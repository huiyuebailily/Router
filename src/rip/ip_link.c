#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "ip_link.h"

#define ip_config_name "ip_config"

IP_LINK *ip_head=NULL;

void save_ip_link(){
	FILE *ip_config = fopen(ip_config_name,"wb+");
	if(ip_config == NULL){
		printf("1Can not open file:%s\n",ip_config_name);
		_exit(1);
	}
	char buff[20]="";
	IP_LINK *pb=ip_head;
	while(pb!=NULL){
		sprintf(buff,"%d.%d.%d.%d\n",pb->ip[0],pb->ip[1],pb->ip[2],pb->ip[3]);
		fputs(buff,ip_config);
		pb = pb->next;
	}
	fclose(ip_config);
}

void init_ip_link(){
	FILE *ip_config = NULL;
	ip_config = fopen(ip_config_name,"rb+");
	if(ip_config == NULL){
		printf("2Can not open file:%s\n",ip_config_name);
		_exit(1);
	}
	while(1){
		char buff[500]="";
		bzero(buff,sizeof(buff));
		int ip;
		if(fgets(buff,sizeof(buff),ip_config)==NULL){
			break;
		}
		buff[strlen(buff)-1]=0;
		inet_pton(AF_INET, buff, &ip);
		IP_LINK *pb = (IP_LINK *)malloc(sizeof(IP_LINK));	
		memcpy(pb->ip, &ip, 4);
		ip_head = inner_ip_link(ip_head,pb);
	}
	printf_ip_link(ip_head);
	fclose(ip_config);
}
void free_ip_link(IP_LINK *head){
	IP_LINK *pb = head;
	while(head){
		pb = head->next;
		free(head);
		head = pb;
	}
}

void printf_ip_link(IP_LINK *head){
	printf("\n---------ip_link_start----------\n");
	IP_LINK *pb = head;
	while(pb!=NULL){
		printf("%d.%d.%d.%d\n", pb->ip[0],pb->ip[1],pb->ip[2],pb->ip[3]);
		pb = pb->next;
	}
	printf("---------ip_link_end----------\n\n");
}
IP_LINK *find_ip(IP_LINK *head, unsigned char *ip){
	IP_LINK *pb = head;
	while(pb){
		if(memcmp(pb->ip,ip,4)==0){
			break;
		}
		pb = pb->next;
	}
	return pb;
}
IP_LINK *inner_ip_link(IP_LINK *head,IP_LINK* p){
	IP_LINK *pb = find_ip(head, p->ip);
	if(pb==NULL){
		p->next = head;
		head = p;
	}else{
		free(p);
	}
	return head;
}

IP_LINK *del_ip_for_link(IP_LINK *head, unsigned char *ip){
	IP_LINK *pf,*pb;
	pf = pb = head;
	while(pb){
		if(memcmp(pb->ip,ip,4)==0){
			break;
		}
		pf = pb;
		pb = pb->next;
	}
	if(pb!=NULL){
		if(pb==head){
			head = head->next;
		}else{
			pf->next = pb->next;
		}
		free(pb);
		pb = NULL;
	}
	return head;
}

