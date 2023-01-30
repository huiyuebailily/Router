#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>//htons
#include <sys/ioctl.h>//ioctl SIOCGIFINDEX
#include <net/if.h>//struct ifreq
#include <netinet/ether.h>//ETH_P_ALL
#include <netpacket/packet.h>
#include "interface.h"
#include "arp.h"
#include "main.h"

#include "arp.h"

int netmask_num =3;	// 3 255	3 netmask

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


ARP_STRUCT *arp_head=NULL;

/******************************************************************
name:		void free_arp_list(ARP_STRUCT *head)
function:		free ARP list
parameter:	ARP_STRUCT *head ARP list head
*******************************************************************/
void free_arp_list(ARP_STRUCT *head){
	ARP_STRUCT *pb = head;
	while(head){
		pb = head->next;
		free(head);
		head = pb;
	}
}


/******************************************************************
name:		void printf_arp_link(ARP_STRUCT *head)
function:		print ARP list to stand output
parameter:	ARP_STRUCT *head ARP list head
*******************************************************************/
void printf_arp_link(ARP_STRUCT *head){
	printf("\n\n---------arp_link_start----------\n");
	ARP_STRUCT*pb = head;
	while(pb){
		printf("%d.%d.%d.%d-->", pb->ip[0],pb->ip[1],pb->ip[2],pb->ip[3]);
		printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
			pb->mac[0],pb->mac[1],pb->mac[2],pb->mac[3],pb->mac[4],pb->mac[5]);
		pb = pb->next;
	}
	printf("---------arp_link_end----------\n\n");
}


/******************************************************************
name:		ARP_STRUCT *find_arp_from_ip(ARP_STRUCT *head, unsigned char *ip)
function:		find mac address by IP address 
parameter:	ARP_STRUCT*head ARP list ,  
		unsigned char *ip IP address
return : 		ARP_STRUCT * 
*******************************************************************/
ARP_STRUCT *find_arp_from_ip(ARP_STRUCT *head, unsigned char *ip){

	ARP_STRUCT *pb = head;

	while(pb){
		if(memcmp(pb->ip,ip,4)==0){
			break;
		}

		pb = pb->next;

	}
	return pb;
}


/******************************************************************
name:		ARP_STRUCT *insert_arp_into_list(ARP_STRUCT *head,ARP_LINK* p)
function:		insert one arp into ARP list 
parameter:	ARP_STRUCT *head ARP list head,  
		unsigned char *ip IP address
return : 		ARP_STRUCT *head ARP list head
*******************************************************************/
ARP_STRUCT *insert_arp_into_list(ARP_STRUCT *head,ARP_STRUCT* p){
	//1. find arp by IP address
	ARP_STRUCT *pb = find_arp_from_ip(head, p->ip);
	if(pb==NULL){
		//2. no arp in arp list, put it in the list head
		p->next = head;
		head = p;
	}else{
		//3. the arp exists, replace the MAC with new
		memcpy(pb->mac,p->mac,6);
	}
	return head;
}


/******************************************************************
name:		void *arp_packet_process(void *arg)
function:		ARP processing thread
*******************************************************************/
void *arp_packet_process(void *arg){
	ARP_STRUCT *p= (ARP_STRUCT *)arg;
	//insert or place new arp into arp list(arp_head)
	arp_head = insert_arp_into_list(arp_head, p);
	return NULL;
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
	 //1. destination IP address 
	memcpy(arp_buf+38, ip, 4);
	//2. source P address 
	memcpy(arp_buf+28, net_interface[network_num].ip, 4); 

	//3. source MAC 
	memcpy(arp_buf+6, net_interface[network_num].mac, 6);

	//source MAC 
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
	int i;
	RECV_DATA *msg_data = (RECV_DATA *)arg;
	//1. get network segment by destiation IP address
	int network_num = find_network_segment((msg_data->data)+30);
	if(network_num==get_interface_num()){
		//1.1 don notsupport the route
		//1.1.1 ENDSYSTEM activate packet
		if (msg_data->data[30] == 255 && msg_data->data[31] == 255 &&
			msg_data->data[32] == 255 &&msg_data->data[33] == 255 )
		{
			ARP_STRUCT *arp;
			arp =(ARP_STRUCT *)malloc(sizeof(ARP_STRUCT));
			for (i=0;i<4;i++)  arp->ip[i] = msg_data->data[26+i];
			for (i=0;i<6;i++)  arp->mac[i] = msg_data->data[6+i];
			arp->next =(struct arp_struct *)NULL;
			arp_packet_process((void *)arp);
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
	ARP_STRUCT  * pb = find_arp_from_ip(arp_head,(msg_data->data)+30);

	//5. if no invalid arp for the destination IP, send arp request t
	if(pb==NULL){

		printf("No routing info about destination:%0d:%0d:%0d:%0d, need asking......\n",
			msg_data->data[30],msg_data->data[31],
			msg_data->data[32],msg_data->data[33]);
		int j;
		//5.1 send 3 times arp request  packet
		//5.1.1 assemble arp request  packet
		assemble_arp_packet((msg_data->data)+30,network_num);
		for(j=0;j<3;j++){
			//5.1.2 send request  packet
			int ret = send_packet(net_interface[network_num].name,arp_buf,sizeof(arp_buf));
			if(ret<0){
				perror("sendmsg1");
			}
			//5.1.3 find arp again, 
			usleep(5000);	//03-26
			pb = find_arp_from_ip(arp_head,(msg_data->data)+30);
			if(pb != NULL){
				break;
			}
			
		}

		if(pb==NULL){
			printf("%d.%d.%d.%d arp error\n",msg_data->data[30],msg_data->data[31],
				msg_data->data[32],msg_data->data[33]);
			return NULL;
		}else{
			printf("%d.%d.%d.%d arp OK\n",msg_data->data[30],msg_data->data[31],
				msg_data->data[32],msg_data->data[33]);
		}
	}
	//6. find valid arp, transfer data 
	//6.1 modify source MAC address
	memcpy((msg_data->data), pb->mac, 6);
	//6.2 modify destination MAC address
	memcpy((msg_data->data)+6, net_interface[network_num].mac, 6);
	//6.3 send the data
	int ret = send_packet(net_interface[network_num].name,msg_data->data,msg_data->data_len);
	if(ret<0){
		printf("msg_data.data_len=%d\n",(int)msg_data->data_len);
		perror("sendmsg2");
	}
	return NULL;
}




