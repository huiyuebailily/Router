#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>//htons
#include <netinet/ether.h>//ETH_P_ALL
#include <pthread.h>
#include "arp.h"
#include "main.h"
#include "ip_link.h"
#include "interface.h"
int main(int argc, char *argv[]){
	//1. get interface information
	init_interface();

	//2 create raw socket
	raw_sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(raw_sock_fd<=0){
		perror("socket");
		_exit(-1);
	}
	char recv_buff[RECV_SIZE]="";
	ssize_t recv_len=0;

	//3 main loop 
	while(1){
		bzero(recv_buff,sizeof(recv_buff));
		//3.1 receive packet from network
		recv_len = recvfrom(raw_sock_fd, recv_buff, sizeof(recv_buff), 0, NULL, NULL);
		if(recv_len<=0||recv_len>RECV_SIZE){
			perror("recvfrom");
			continue;
		}
		//3.2 process ARP packet()
		if((recv_buff[12]==0x08)&&(recv_buff[13]==0x06)){
			ARP_STRUCT *p = (ARP_STRUCT *)malloc(sizeof(ARP_STRUCT));
			if(p==NULL){
				perror("malloc");
				continue;
			}
			memcpy(p->mac, recv_buff+22, 6);//mac
			memcpy(p->ip , recv_buff+28, 4);//ip
			//printf("%d.%d.%d.%d-->",p->ip[0],p->ip[1],p->ip[2],p->ip[3]);
			pthread_t ARP_T;
			pthread_create(&ARP_T, NULL,arp_packet_process, (void*)p);
			pthread_detach(ARP_T);
		}
		//3.3 process IP packet
		if((recv_buff[12]==0x08)&&(recv_buff[13]==0x00)){
			//3.3.1 find ip destination 
			IP_LINK *ip_pb = find_ip(ip_head, (unsigned char*)recv_buff+30);
			if(ip_pb!=NULL){
				//can not process
				continue;
			}
			RECV_DATA *recv = (RECV_DATA *)malloc(sizeof(RECV_DATA));
			recv->data_len = recv_len;
			memcpy(recv->data, recv_buff, recv_len);

			//5.3.2 create a new thread to transfer
			pthread_t IP_T;
			pthread_create(&IP_T, NULL,transfer_data, (void*)recv);
			pthread_detach(IP_T);
		}
	}
	
	return 0;
}