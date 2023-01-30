#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>//htons
#include <netinet/ether.h>//ETH_P_ALL
#include <pthread.h>
#include "arp.h"
#include "main.h"
#include "ip_link.h"
#include "interface.h"


void init_routing_table();
void rip_init();
void main_loop();
void rip_process(int );
void print_routing_table();
extern time_t last_time_broadcast;
int judge_and_process_rip_packet(int recv_len ,char *recv_buff,char *ip);
int main(int argc, char *argv[]){

	int flag,retcode;
	char	dotip[60];
	int addr_len,counter;
	struct sockaddr_in addr;
	//struct sockaddr_in addr6;

	//1 initianization 
	//1. 1init route table and rip info
	init_routing_table();
	rip_init();


	//1.2. get interface information
	init_interface();

	
	//1.3 create raw socket
	raw_sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(raw_sock_fd<=0){
		perror("socket");
		_exit(-1);
	}
	//1.3.1 .set nonblock
	flag = fcntl(raw_sock_fd , F_GETFL, 0);
	if (flag < 0) {
		//Perror("fcntl F_GETFL fail\n");
        		return -1;
	}
	if (fcntl(raw_sock_fd, F_SETFL, flag | O_NONBLOCK) < 0) {
       		printf("fcntl F_SETFL fail\n");
	}


	char recv_buff[RECV_SIZE]="";
	ssize_t recv_len=0;

	//2 main loop 
	counter = 0;
	while(1){
		usleep(100);
		rip_process(counter);
		counter ++;
		if (counter < 0)
		counter = 1;

		bzero(recv_buff,sizeof(recv_buff));
		//5.1 receive packet from network
		//recv_len = recvfrom(raw_sock_fd, recv_buff, sizeof(recv_buff), 0, NULL, NULL);
		recv_len = recvfrom(raw_sock_fd, recv_buff, sizeof(recv_buff), 0, (struct sockaddr*)&addr, (socklen_t*)&addr_len);
		//source_addr.s_addr = addr.sin_addr.s_addr;
		//memcpy((char*)&addr6.sin_addr,(char*)&addr.sin_addr,sizeof(addr.sin_addr));
		if(recv_len<=0||recv_len>RECV_SIZE){
			//perror("recvfrom");
			continue;
		}
		//sprintf(dotip,"%s",inet_ntoa(addr.sin_addr));
		sprintf(dotip,"%0d.%0d.%0d.%0d",recv_buff[26],recv_buff[27],recv_buff[28],recv_buff[29]);

		//5.2 process ARP packet()
		if((recv_buff[12]==0x08)&&(recv_buff[13]==0x06)){

			//printf("bbbbb SRC IP:%0d,%0d,%0d,%0d, DEST IP:%0d,%0d,%0d,%0d,UDP=%0d,dest port=%0x,%0x\n",
			//	recv_buff[26],recv_buff[27],recv_buff[28],recv_buff[29],
			//	recv_buff[30],recv_buff[31],recv_buff[32],recv_buff[33],
			//	recv_buff[23],recv_buff[36],recv_buff[37]);
			//print_routing_table();

			struct route_struct rs;
			memcpy(rs.mac, recv_buff+22, 6);	//mac address
			memcpy(rs.ip , recv_buff+28, 4);	//ip address
			pthread_t ARP_T;
			pthread_create(&ARP_T, NULL,arp_packet_process, (void*)&rs);
			pthread_detach(ARP_T);
			//arp_packet_process((void *)&rs);
		}


		//5.3 process IP packet
		if((recv_buff[12]==0x08)&&(recv_buff[13]==0x00)){

			//judge if RIP request or answer
			retcode = judge_and_process_rip_packet(recv_len ,recv_buff,dotip);
			if (retcode == 1)
			{//is RIP 

				continue;
			}
			//5.3.1 find ip destination 
			IP_LINK *ip_pb = find_ip(ip_head, (unsigned char*)recv_buff+30);
			if(ip_pb!=NULL){

				//can not process
				continue;
			}
			RECV_DATA *recv = (RECV_DATA *)malloc(sizeof(RECV_DATA));
			recv->data_len = recv_len;
			memcpy(recv->data, recv_buff, recv_len);
			//5.3.2 create a new thread to transfer

			//pthread_t IP_T;
			//pthread_create(&IP_T, NULL,transfer_data, (void*)recv);
			//pthread_detach(IP_T);
			transfer_data(recv);
		}

	}
	
	return 0;
}