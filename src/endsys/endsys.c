#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <unistd.h>   
#include <errno.h>   
#include <string.h>
#include <stdlib.h>   
#include <string.h>   
#include <arpa/inet.h>   
#include <fcntl.h>
#include<pthread.h>
  
#define PORT_NO   9999  
 
//#define DEST_PORT 9999 
int	sock_send;
int	sock_recv;
int	global_ttl;
int	valid_ip;
char	dest_ip[60];
char	local_ip[60];
struct sockaddr_in addr_recv;  
struct sockaddr_in addr_send;  

void* receive_mesg(void* arg);
int receive_init();
int send_init();
int send_mesg(int first_time)  ;


void print_usage()
{
	printf("usuage:  task1 LOCALIPADDRES TTL     \n");
}

int main(int argc, char **argv)  
{ 
	if (argc == 3)
	{	int ret, i1,i2,i3,i4;
		ret = sscanf(argv[1],"%d.%d.%d.%d", &i1,&i2,&i3,&i4);
		if (ret != 4)
		{
			printf("Invalid local IP address, use default\n");
		}
		else strcpy(local_ip,argv[1]);
		ret = sscanf(argv[2],"%d", &global_ttl);
		if (ret != 1 || global_ttl < 0 || global_ttl > 10)
		{
			printf("Invalid TTL, use default =1\n");
			global_ttl = 1;
		}
	}
	else if (argc == 1)
	{
		global_ttl = 1;
		local_ip[0] = 0;
	}
	else
	{
		print_usage();
		exit(0);
	}


	global_ttl = 0;
	dest_ip[0] = 0;
	valid_ip = 0;
	send_init() ;
	receive_init();


	pthread_t tid;
	int retcode;
	retcode = pthread_create(&tid, NULL, receive_mesg, NULL);
	//if(retcode != 0)
	//	printf("%s: %d\n",__func__, strerror(retcode));
	int i;
	send_mesg(1) ;
	for (;;)
	{
		send_mesg(0) ;
		//receive_mesg();
	}


}
int receive_init()
{
	int len;
	//1. create udp sock_fd
	sock_recv= socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_recv < 0)  
	{  
		perror("socket");  
		exit(1);  
	}  

	 //2. generte port and IP address info, and then bind them to socket
	//struct sockaddr_in addr_serv;  
	
	memset(&addr_recv, 0, sizeof(struct sockaddr_in)); 	 //reset to 0
	addr_recv.sin_family = AF_INET;			//IPV4
	addr_recv.sin_port = htons(PORT_NO);			//port no=9999
	// INADDR_ANY :can receive all info to this machine if to port =SERVICE_PORT 
	addr_recv.sin_addr.s_addr = htonl(INADDR_ANY);  
	len = sizeof(addr_recv);  

	//2.2 bind 
	if(bind(sock_recv, (struct sockaddr *)&addr_recv, sizeof(addr_recv)) < 0)  {  
		perror("bind error:");  
		exit(1);  
	 }  


	int flag = fcntl(sock_recv, F_GETFL, 0);
   	if (flag < 0) {
		perror("fcntl F_GETFL fail");
		return -1;
	}
	if (fcntl(sock_recv, F_SETFL, flag | O_NONBLOCK) < 0) {
		perror("fcntl F_SETFL fail");
		return -1;
	}

	return 0;  
}

int send_init()  
{ 
	//int sock_fd;  

	//1. create udp sock_fd
	sock_send = socket(AF_INET, SOCK_DGRAM, 0);  
	if(sock_send < 0)  
	{  
		perror("socket");  
		exit(1);  
	}  


	//2. generte port and destination IP address info,
	//struct sockaddr_in addr_serv;  
	int len;  
	memset(&addr_send, 0, sizeof(addr_send));  
	addr_send.sin_family = AF_INET;  
	//addr_send.sin_addr.s_addr = inet_addr(ip_address);  
	addr_send.sin_port = htons(PORT_NO);  
	int flag = 1;
	int ret2= setsockopt(sock_send   , SOL_SOCKET,SO_REUSEADDR | SO_BROADCAST,&flag  ,sizeof(flag  ));
	//printf("SET BROAD  :%d\n",ret2);
	//int send_num,len; 
	//len = sizeof(addr_send);  
  
	return 0;  
}


void* receive_mesg(void* arg)
{

	int i;
	for (i=0;;i++)
	{

		int recv_num,len;     
		char recv_buf[600];  
		struct sockaddr_in addr_client;  

		recv_num = recvfrom(sock_recv, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);  

		if(recv_num < 0)  
		{  

			sleep(1);
			continue ;
		}  

		recv_buf[recv_num] = '\0';  
		if (!strncmp(recv_buf,"INITIALIZE",strlen("INITIALIZE")))
			printf("%s is active\n",inet_ntoa(addr_client.sin_addr));
		else printf("Receive message from:%s, len=%d, detail: %s\n", inet_ntoa(addr_client.sin_addr),recv_num, recv_buf);  
		usleep(100);
	}
	
	return 0;  
}



int send_mesg(int first_time)  
{ 
	//int sock_fd;  
	int ret,i1,i2,i3,i4,ttl;
	char send_buf[600];

	int send_num,len; 
	len = sizeof(addr_send);  
	 
	if (first_time == 1)
	{
		strcpy(send_buf,"INITIALIZE");
		addr_send.sin_addr.s_addr = inet_addr("255.255.255.255");  
		unsigned char ttl = 0;//64; 
		ret = setsockopt(sock_send, IPPROTO_IP, IP_MULTICAST_TTL,(char *)&ttl, sizeof(unsigned char));
		//printf("aaaaaaaaaaa:%d\n",ret);
	}
	else 
	{
		//int valid_ip;
		
		//if (dest_ip[0] == 0)
		do
		{
			if (valid_ip == 1)
			printf("Please input dest IP address (Invalid input will remain using  last address: %s :",dest_ip); 
			else 
			printf("Please input dest IP address (for example: 10.0.0.1) :");  

			scanf("%s",send_buf);
			//i = 0;
			//while((send_buf[i++] =getchar())!='\n') ;
			//send_buf[i] = '\0';


			ret = sscanf(send_buf,"%d.%d.%d.%d", &i1,&i2,&i3,&i4);
			if (ret == 4){
				//printf("IIII %d,%d,%d,%d\n",i1,i2,i3,i4);
				valid_ip= 1;
				sprintf(dest_ip,"%d.%d.%d.%d", i1,i2,i3,i4);
				addr_send.sin_addr.s_addr = inet_addr(dest_ip);  
			}
		}
		while (valid_ip == 0);
		printf("Please input message for sending :");  
		scanf("%s",send_buf);
		//i = 0;
		//while((send_buf[i++] =getchar())!='\n') ;
	}
	send_num = sendto(sock_send, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_send, len);  
  	//printf("send ret=%d,%d\n",send_num ,strlen(send_buf) );
	if(send_num < 0)  
	{  
		perror("sendto error:");  
		//exit(1);  
	}  
	if (first_time == 1)
	{
		unsigned char ttl = global_ttl;//64;
		ret = setsockopt(sock_send, IPPROTO_IP, IP_MULTICAST_TTL,(char *)&ttl, sizeof(unsigned char));
		//printf("bbbbbbbbbbbb:%d\n",ret);
	}
	//close(sock_fd);  
   
	return 0;  
}
