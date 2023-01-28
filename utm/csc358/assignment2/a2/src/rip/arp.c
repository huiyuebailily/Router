#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>//htons
#include <sys/ioctl.h>//ioctl SIOCGIFINDEX
#include <net/if.h>//struct ifreq
#include <netinet/ether.h>//ETH_P_ALL
#include <netpacket/packet.h>
#include "interface.h"
#include "arp.h"
#include "main.h"

#include "arp.h"

int netmask_num =3;	// 3 255	

void print_routing_table();
//ARP struct 					
unsigned char arp_buf[42] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 	// dst MAC	[0]-->[5]
	0, 0, 0, 0, 0, 0, 			// src MAC	[6]-->[11]
	0x08, 0x06, 			// ARP	[12]-->[13]
	0x00, 0x01, 			// Ethernet  [14]-->[15]
	0x08, 0x00, 			// IP  [16]-->[17]
	0x06,				// HW address size	[18]
	0x04,				// protocol(IP here) address size  [19]
	0x00, 0x01, 			// opcode : 1.arp request 2.arp answer 3.rarp request 4.rarp answer  [20]-->[21]
	0, 0, 0, 0, 0, 0, 			// src MAC	[22]-->[27]
	0, 0, 0, 0,				// src ip : 192.168.1.254  [28]-->[31]
	0, 0, 0, 0, 0, 0,			// dst MAC	[32]-->[37]
	0, 0, 0, 0				// dst ip :   [38]-->[41]
	};


//ARP_STRUCT *arp_head=NULL;
//full routing list
struct route_struct route_table[ROUTE_MAX_NUM ];	



/******************************************************************
name:		void printf_arp_link(ARP_STRUCT *head)
function:		print ARP list to stand output
parameter:	
*******************************************************************/
void printf_arp_link(){
	int i;

	printf("\n\n---------arp_link_start----------\n");
	for (i=0;i<ROUTE_MAX_NUM ;i++)
	{
		printf("%d.%d.%d.%d-->", route_table[i].ip[0],route_table[i].ip[1],route_table[i].ip[2],route_table[i].ip[3]);
		printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
			route_table[i].mac[0],route_table[i].mac[1],route_table[i].mac[2],
			route_table[i].mac[3],route_table[i].mac[4],route_table[i].mac[5]);
	}
	printf("---------arp_link_end----------\n\n");
}


/******************************************************************
name:		int find_arp_from_ip(ARP_STRUCT *head, unsigned char *ip)
function:		find mac address by IP address 
parameter:	unsigned char *ip IP address
return : 		<0 or >= ROUTE_MAX_NUM :error
		else: the position in route_table for new IP
*******************************************************************/
int  find_arp_from_ip(unsigned char *ip){
	int i;

	for (i=0;i<ROUTE_MAX_NUM;i++)
	{
		if (memcmp(route_table[i].ip,ip,4)==0 && route_table[i].metric != -1){
			//the ip exists
			return i;
		}
	}

	for (i=0;i<ROUTE_MAX_NUM;i++)
	if (route_table[i].metric < 0){
		//the ip apppears first time
		return i;
	}
	return -1;
}


/******************************************************************
name:		int insert_arp_into_list(ARP_STRUCT *head,ARP_LINK* p)
function:		insert one arp into ARP list 
parameter:	unsigned char *ip IP address
return : 		-1:	error
		0<= and <ROUTE_MAX_NUM:succeed
*******************************************************************/
int insert_local__into_list(struct route_struct* rs){
	int ret,retcode;
	char dotip[160];

	//1. find arp by IP address
	ret = find_arp_from_ip(rs->ip);
	if(ret  < 0 || ret  >= ROUTE_MAX_NUM ){
		//2. no route table full
		return -1;
	}else{
		//3. save IP and MAC into route_table
		//memcpy(pb->mac,p->mac,6);
		//transfer char * ip to struct ip
		sprintf(dotip,"%0d.%0d.%0d.%0d",rs->ip[0],rs->ip[1],rs->ip[2],rs->ip[3]);
		retcode = inet_pton(AF_INET, (const char *)dotip, &(route_table[ret].dest));
		if (retcode  <= 0)
		{
			printf("Invalid IP:%0x.%0x.%0x.%0x, MAC::%0x.%0x.%0x.%0x:%0x.%0x, can not process\n",
				rs->ip[0],rs->ip[1],rs->ip[2],rs->ip[3],
				rs->mac[0],rs->mac[1],rs->mac[2],rs->mac[3],rs->mac[4],rs->mac[5]);

			return -1;
		}
		inet_pton(AF_INET, (const char *)dotip, &(route_table[ret].next));
		if (route_table[ret].metric < 1)
		{//local route info ,occur first time
			printf("New local route info, no=%d,metric=%d, IP:%0d.%0d.%0d.%0d, MAC::%0x.%0x.%0x.%0x:%0x.%0x\n",
				ret,route_table[ret].metric,
				rs->ip[0],rs->ip[1],rs->ip[2],rs->ip[3],
				rs->mac[0],rs->mac[1],rs->mac[2],rs->mac[3],rs->mac[4],rs->mac[5]);
		}
		memcpy(route_table[ret].mac,rs->mac,6);	
		memcpy(route_table[ret].ip,rs->ip,4);		
		//memset((char*)&route_table[ret].dest,0,sizeof(route_table[ret].dest));
		//memset((char*)&route_table[ret].next,0,sizeof(route_table[ret].next));
		route_table[ret].dest.s_addr = inet_addr(dotip);  
		route_table[ret].metric = 1;			//local route info
		route_table[ret].valid = 1;			//recently renew
		route_table[ret].flag = 1;			//trigger
		time(&route_table[ret].time);
		return ret;
	}
	return -1;
}


/******************************************************************
name:		void *arp_packet_process(void *arg)
function:		ARP processing thread
*******************************************************************/
void *arp_packet_process(void *arg){

	int iftrue;

	struct route_struct *rs= (struct route_struct *)arg;
	//insert or place new arp into arp list(arp_head)
	 iftrue = (int)(rs->ip[3] != 0 && rs->ip[3] != 1 && rs->ip[3] != 2 && rs->ip[3] != 3);
	if (iftrue != 0 )
	{
//printf("zzzzzzzz:%0d.%0d.%0d.%0d---%d,%d,%d\n",rs->ip[0],rs->ip[1],rs->ip[2],rs->ip[3],(rs->ip[3] != 0),
//	(rs->ip[3] != 0 && rs->ip[3] != 1 && rs->ip[3] != 2 && rs->ip[3] != 3),iftrue );
		insert_local__into_list(rs);
	}

	return NULL ;
}



/******************************************************************
name:		int find_network_segment(unsigned char *ip)
function:		find network_segmen by ip
parameter:	unsigned char *ip destination ip address
return : 		network segment
*******************************************************************/	
int find_network_segment(unsigned char *ip){
	int i=0,j=get_interface_num();
	for(i=0;i<j;i++){
		//compare IP address according to netmask
		if(memcmp(net_interface[i].ip, ip, netmask_num)==0)
			break;
	}
	return i;
}

/******************************************************************
name:		int send_packet(int network_num,unsigned char *msg_data,unsigned long msg_data_len)
function:		transfer data by network no
parameter:	int network_num 
		unsigned char *msg_data , data need to be transfered
		unsigned long msg_data_len 
return: 		data length
*******************************************************************/
int send_packet(char *eth_name,unsigned char *msg_data,unsigned long msg_data_len){
	struct ifreq ethrep;
	strncpy(ethrep.ifr_name, eth_name,IFNAMSIZ);
	if(-1 == ioctl(raw_sock_fd,SIOGIFINDEX,&ethrep)){
		perror("send_msg-ioctl");
		close(raw_sock_fd);
		_exit(-1);
	}
	struct sockaddr_ll sll;
	bzero(&sll,sizeof(sll));
	sll.sll_ifindex = ethrep.ifr_ifindex;
	int len = sendto(raw_sock_fd, msg_data, msg_data_len, 0,(struct sockaddr*)&sll, sizeof(sll));
	return len;
}


/******************************************************************
name:		void assemble_arp_packet(unsigned char *ip,int network_num)
function:		assemble ARP  packet
parameter:	unsigned char *ip ,destination IP address
		int network_num,network no
*******************************************************************/
void assemble_arp_packet(unsigned char *ip,int network_num){
	 //1. destination IP address 目的IP
	memcpy(arp_buf+38, ip, 4);
	//2. source P address 源ip
	memcpy(arp_buf+28, net_interface[network_num].ip, 4); 

	//3. source MAC 源mac
	memcpy(arp_buf+6, net_interface[network_num].mac, 6);

	//source MAC 源mac
	memcpy(arp_buf+22, net_interface[network_num].mac, 6);
}


/******************************************************************
name:		void *transfer_data(void *arg)
function:		IP thread
		select net interface according to network segment
		for destination with no MAC , send ARP request   packet
parameter:	void *arg ,data need to be transfered
*******************************************************************/

void *transfer_data(void *arg){
	int i,ret;
	RECV_DATA *msg_data = (RECV_DATA *)arg;

	//1. get network segment by destiation IP address
	int network_num = find_network_segment((msg_data->data)+30);
	if(network_num==get_interface_num()){
		//1.1 don notsupport the route
		//1.1.1 ENDSYSTEM activate packet
		if (msg_data->data[30] == 255 && msg_data->data[31] == 255 &&
			msg_data->data[32] == 255 &&msg_data->data[33] == 255 )
		{

			struct route_struct rs;
			for (i=0;i<4;i++)  rs.ip[i] = msg_data->data[26+i];
			for (i=0;i<6;i++)  rs.mac[i] = msg_data->data[6+i];
			arp_packet_process((void *)&rs);
#if 0
		printf("SRCMAC1:%0x:%0x:%0x:%0x:%0x:%0x  SRCIP= %d.%d.%d.%d dest=%d.%d.%d.%d,pp=%0x,%0x \n",
			msg_data->data[6],msg_data->data[7],
			msg_data->data[8],msg_data->data[9],
			msg_data->data[10],msg_data->data[11],

			msg_data->data[26],msg_data->data[27],
			msg_data->data[28],msg_data->data[29],
			msg_data->data[30],msg_data->data[31],
			msg_data->data[32],msg_data->data[33],
			msg_data->data[36],msg_data->data[37]);
#endif
			print_routing_table();
		}
		return NULL;
	}

	//2.  not transfer broadcast package
	if(msg_data->data[33]==255){
		printf("%d.%d.%d.%d broadcast\n",msg_data->data[30],msg_data->data[31],
				msg_data->data[32],msg_data->data[33]);
		return NULL;
	}
	//3. not transfer loopback 
	if(strcmp(net_interface[network_num].name,"lo")==0){
		return NULL;
	}

	//4. find arp info by destination IP address 
	ret = find_arp_from_ip((msg_data->data)+30);
	//5. if no invalid arp for the destination IP, send arp request t
	int need_request = 0;
	if (ret < 0) 
	{
		printf("SRCMAC2:%0x:%0x:%0x:%0x:%0x:%0x  SRCIP= %d.%d.%d.%d dest=%d.%d.%d.%d \n",
			msg_data->data[6],msg_data->data[7],
			msg_data->data[8],msg_data->data[9],
			msg_data->data[10],msg_data->data[11],

			msg_data->data[26],msg_data->data[27],
			msg_data->data[28],msg_data->data[29],
			msg_data->data[30],msg_data->data[31],
			msg_data->data[32],msg_data->data[33]);
		need_request =1;
	}
	else if (route_table[ret].metric < 0) 
	{
#if 0
		printf("SRCMAC3:%0x:%0x:%0x:%0x:%0x:%0x  SRCIP= %d.%d.%d.%d dest=%d.%d.%d.%d \n",
			msg_data->data[6],msg_data->data[7],
			msg_data->data[8],msg_data->data[9],
			msg_data->data[10],msg_data->data[11],

			msg_data->data[26],msg_data->data[27],
			msg_data->data[28],msg_data->data[29],
			msg_data->data[30],msg_data->data[31],
			msg_data->data[32],msg_data->data[33]);
#endif
		need_request =1;
	}

	if(need_request  == 1){

		//printf("No routing info about destination:%0d:%0d:%0d:%0d, need asking......\n",
		//	msg_data->data[30],msg_data->data[31],
		//	msg_data->data[32],msg_data->data[33]);
		int j;
		//5.1 send 3 times arp request  packet
		//5.1.1 assemble arp request  packet
		assemble_arp_packet((msg_data->data)+30,network_num);
		for(j=0;j<3;j++){
			//5.1.2 send request  packet
			int ret = send_packet(net_interface[network_num].name,arp_buf,sizeof(arp_buf));//发送arp消息
			if(ret<0){
				perror("sendmsg1");
			}
			//5.1.3 find arp again, 
			usleep(500);	//03-26
			ret = find_arp_from_ip((msg_data->data)+30);
			need_request = 0;
			if (ret < 0) need_request =1;
			else if (route_table[ret].metric < 0)  need_request =1;

			if(need_request  == 0){
				break;
			}
			
		}

		if(ret <0){
			printf("%d.%d.%d.%d arp error\n",msg_data->data[30],msg_data->data[31],
				msg_data->data[32],msg_data->data[33]);
			return NULL;
		}else{
			//printf("%d.%d.%d.%d arp OK\n",msg_data->data[30],msg_data->data[31],
			//	msg_data->data[32],msg_data->data[33]);
		}
	}
	//6. find valid arp, transfer data 
	//6.1 modify source MAC address
	memcpy((msg_data->data), route_table[ret].mac, 6);
	//6.2 modify destination MAC address
	memcpy((msg_data->data)+6, net_interface[network_num].mac, 6);

	//6.3 send the data
	ret = send_packet(net_interface[network_num].name,msg_data->data,msg_data->data_len);
	if(ret<0){
		printf("msg_data.data_len=%d\n",(int)msg_data->data_len);
		perror("sendmsg2");
	}
	return NULL;
}




