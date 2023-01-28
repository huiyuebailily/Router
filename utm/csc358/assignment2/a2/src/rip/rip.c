#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>
#include "arp.h"
#include "rip.h"
#include "interface.h"

#define RIP_PORT	520


int gsock ;
time_t last_time_broadcast;
void print_rip_packet();

void get_cur_time_str(char *time_str)
{
	time_t cur_time;
 	struct tm tm6;
	
	time(&cur_time);
      	localtime_r(&cur_time,&tm6 );

       	sprintf( time_str,"%2.2d%2.2d%2.2d", tm6.tm_hour, tm6.tm_min,tm6.tm_sec);
}



void init_socket()
{	
	//int retcode;
	int val = 1;
	gsock  = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(gsock , SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));

	broadcast_sock  = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(broadcast_sock , SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	//val = 0;
	//retcode = setsockopt(broadcast_sock  ,IPPROTO_IP,IP_MULTICAST_LOOP,(const char*)&val,sizeof(val));
	//printf("broadcast set multicat=%d\n",retcode);
}


void rp_inverse_ntoh()
{
	int i;
	void * tempptr =  & current_rip_packet.command;
	unsigned short * temp = (unsigned short *)( tempptr );
	*temp = ntohs(* temp);
	for(i=0; i < current_rip_count; i++)
	{
		current_rip_packet.rip_entries[i].metric = ntohl(current_rip_packet.rip_entries[i].metric);
	}
}

void rp_inverse_hton()
{
	void * tempptr =  & current_rip_packet.command;
	unsigned short * temp = (unsigned short *)( tempptr );

	*temp = htons(* temp);
	for(int i=0; i < current_rip_count; i++)
	{
		current_rip_packet.rip_entries[i].metric = htonl(current_rip_packet.rip_entries[i].metric);
	}
}




void print_message(const char* title, const char* content)
{
	printf("- %s -\n", content);
}





void reset_rip_packet(int command)
{
	current_rip_count = 0;
	current_rip_packet.command = command;
	current_rip_packet.version = 1;
	current_rip_packet.zero = 0;
	for (int i = 0; i < RIP_MAX_ENTRY; i++)
	{
		current_rip_packet.rip_entries[0].family = 2;
		current_rip_packet.rip_entries[0].zero1 = 0;
		current_rip_packet.rip_entries[0].ip.s_addr = 0;
		current_rip_packet.rip_entries[0].zero2 = 0;
		current_rip_packet.rip_entries[0].zero3 = 0;
		current_rip_packet.rip_entries[0].metric = 0;
	}
}


void init_routing_table()
{
	int i;

	for (i=0;i<ROUTE_MAX_NUM;i++)
	{
		route_table[i].metric = -1;	//means not used, valid route info
	}

}

void rip_init()
{
	init_socket();
	reset_rip_packet(RIP_RESPONSE);
	time(&last_time_broadcast);
}

void gen_rip_request_packet()
{
	current_rip_count = 1;
	current_rip_packet.command = RIP_REQUEST;
	current_rip_packet.version = 1;
	current_rip_packet.zero = 0;
	current_rip_packet.rip_entries[0].family = 0;
	current_rip_packet.rip_entries[0].zero1 = 0;
	current_rip_packet.rip_entries[0].ip.s_addr = 0;
	current_rip_packet.rip_entries[0].zero2 = 0;
	current_rip_packet.rip_entries[0].zero3 = 0;
	current_rip_packet.rip_entries[0].metric = RIP_INFINITY;
}
#if 0
void send_rip_packet(char *srcip)
{
	struct sockaddr_in addr;

//printf("Send rip packet , rip num=%d\n",current_rip_count);
	print_rip_packet();
	rp_inverse_hton();

	int addr_len = sizeof(struct sockaddr_in);
	bzero(&addr, addr_len);
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = htonl(dest_ip.s_addr);
	//inet_aton(srcip,&addr.sin_addr.s_addr);
	inet_aton(srcip,&addr.sin_addr);


	addr.sin_port = htons(RIP_PORT);
	if (sendto(gsock , &current_rip_packet, RIP_PACKET_HEAD+   current_rip_count*sizeof(struct rip_entry),0,
		(const struct sockaddr*)&addr, addr_len) < 0)
	{
		//printf("sent green in send_rip_packet_to\n");
		;
	}
	
	return;
}
#endif

void send_rip_packet(char *srcip)
{
	int i,addr_len;
	char dotip[60];
	struct sockaddr_in addr;

//printf("Send rip packet , rip num=%d\n",current_rip_count);
	print_rip_packet();
	rp_inverse_hton();
	addr_len = sizeof(struct sockaddr_in);

	for (i=0;i<interface_num;i++)
	if (strcmp(net_interface[i].name,"lo"))
	{
		bzero(&addr, addr_len );
		addr.sin_family = AF_INET;
		sprintf(dotip,"%0d.%0d.%0d.%0d",net_interface[i].br_ip[0],net_interface[i].br_ip[1],
			net_interface[i].br_ip[2],net_interface[i].br_ip[3]);
		addr.sin_addr.s_addr = inet_addr(dotip); 
		addr.sin_port = htons(RIP_PORT);

		addr.sin_port = htons(RIP_PORT);
		if (sendto(gsock , &current_rip_packet, RIP_PACKET_HEAD+   current_rip_count*sizeof(struct rip_entry),0,
			(const struct sockaddr*)&addr, addr_len ) < 0)
		{
			//printf("sent green in send_rip_packet_to\n");
			;
		}
	}
	return;
}

void broadcast_rip_request()
{
	int i,len;
	time_t cur_time;
	char dotip[60];


	print_rip_packet();
	rp_inverse_hton();
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	time(&cur_time);
	//printf("Broadcast RIP, time=%d,current_rip_count=%d\n",(int)cur_time,current_rip_count);
	for (i=0;i<interface_num;i++)
	if (strcmp(net_interface[i].name,"lo"))
	{

		bzero(&addr, addr_len);
		addr.sin_family = AF_INET;
		sprintf(dotip,"%0d.%0d.%0d.%0d",net_interface[i].br_ip[0],net_interface[i].br_ip[1],
			net_interface[i].br_ip[2],net_interface[i].br_ip[3]);
		addr.sin_addr.s_addr = inet_addr(dotip); 
		addr.sin_port = htons(RIP_PORT);

		len = sendto(broadcast_sock    , &current_rip_packet,RIP_PACKET_HEAD+current_rip_count*sizeof(struct rip_entry),
			0, (struct sockaddr*)&addr, addr_len) ;
		if (len< 0)
		{
			printf("socket error in broadcast_rip_request:%d,%d\n",broadcast_sock ,errno);
		}
	}
	return;
}


void update_routing_table()
{
	time_t cur_time;

	time(&cur_time);
	for (int i = 0; i <ROUTE_MAX_NUM; i++)
	if (route_table[i].metric >= 1)
	{//valid route
		if( cur_time -route_table[i].time >= (time_t)GARBAGE_REMOVE_TIMEOUT && route_table[i].metric !=1)
		{//noued
			route_table[i].metric = -1;
			route_table[i].valid = 0;
		}
		else if( cur_time -route_table[i].time>= (time_t)CLOCK_TIMEOUT)
		{//timeout
			route_table[i].valid = 0;
		}
	}

}

int calculate_rip_metric(int rip_entry_number)
{
	if (current_rip_packet.rip_entries[rip_entry_number].metric + 1 > RIP_INFINITY)
		return RIP_INFINITY;
	return current_rip_packet.rip_entries[rip_entry_number].metric + 1;
}


void process_rip_response(char *srcip)
{
	int i,j,k,del_t;
	int changed_num;
	int route_exist = 0;
	time_t cur_time;
	char tt1[60],tt2[60];

	//int old_route_count = current_route_count;
	changed_num = 0;
	//printf("receive rip response, from:%s,num=%d\n",srcip, current_rip_count);
	for (i = 0; i < current_rip_count; i++)
	{
		//printf("xxxxxxxxx1 :%d, receive rip, from:%s,num=%d\n",i,inet_ntoa(source_ip), current_rip_count);
		route_exist = 0;
		for (j=0;j<ROUTE_MAX_NUM;j++)
		if (current_rip_packet.rip_entries[i].ip.s_addr == route_table[j].dest.s_addr )
		{//route_table[j].metric local address
			if (route_table[j].metric == 1)
			{
				route_exist = 1;
				break;
			}

			route_exist = 1;
			if((current_rip_packet.rip_entries[i].ip.s_addr == route_table[j].dest.s_addr )
				&& ( route_table[j].metric > calculate_rip_metric(i)))
			{
				//int ttt= inet_aton(srcip,&route_table[j].next.s_addr);
				inet_aton(srcip,&route_table[j].next);
				route_table[j].metric = calculate_rip_metric(i);
				strcpy(tt1,inet_ntoa(route_table[j].dest));
				strcpy(tt2,inet_ntoa(route_table[j].next));
				//strcpy(tt3,inet_ntoa(source_ip));
				route_table[j].time = 0;
				printf("Modify RIP,no=%d, dest:%s,next:%s,%s,metric:%d\n",j,tt1,tt2,srcip,route_table[j].metric);
			}
			time(&cur_time);
			del_t = cur_time - route_table[j].time;
			if (del_t > 1)
			{
				route_table[j].time =cur_time ;
				changed_num++;
			}
			route_table[j].valid = 1;
			route_table[j].flag = 1;/// trigger...
			break;
		}
		if (route_exist == 0 && calculate_rip_metric(i) != RIP_INFINITY)
		{
			k = -1;
			for (j=0;j<ROUTE_MAX_NUM;j++)
			if (route_table[j].metric < 1)
			{
				k = j;
				break;
			}
			if (k != -1)
			{
				route_table[k].dest.s_addr = current_rip_packet.rip_entries[i].ip.s_addr;
				//route_table[k].next.s_addr = source_ip.s_addr;
				//int ttt= inet_aton(srcip,&route_table[k].next.s_addr);
				memcpy(route_table[k].ip,(char*)&route_table[k].dest.s_addr,4);
				inet_aton(srcip,&route_table[k].next);
				route_table[k].metric = calculate_rip_metric(i);
				time(&route_table[k].time);
				route_table[k].valid = 1;
				route_table[k].flag = 1;//to be trigger...

				strcpy(tt1,inet_ntoa(route_table[k].dest));
				strcpy(tt2,inet_ntoa(route_table[k].next));
				//strcpy(tt3,inet_ntoa(source_ip));
				changed_num++;
				char tmstr[60];
				get_cur_time_str(tmstr);

				printf("%s: New RIP2,no=%d, dest:%s,next:,%s,metric:%d\n",tmstr,
					k,tt1,srcip,route_table[k].metric);
			}
		}
	}
	current_rip_count = 0;
	reset_rip_packet(RIP_RESPONSE);
	for (k=0;k<ROUTE_MAX_NUM;k++)
	{
		if( (route_table[k].flag == 1 && route_table[k].metric >= 1) ||  route_table[k].metric == 1)
		{
			current_rip_packet.rip_entries[current_rip_count].ip.s_addr =route_table[k].dest.s_addr;
			current_rip_packet.rip_entries[current_rip_count].metric = route_table[k].metric;
			current_rip_count++;
			current_rip_packet.command = RIP_RESPONSE;
			if (route_table[k].metric != 1)
			route_table[k].flag = 0;
		}
	}
	if (changed_num > 0)
	{
		time(&cur_time);
		//printf("11SEND RIP time=%d\n",(int)cur_time);
		send_rip_packet(srcip);
	}
}


void process_rip_request(char *srcip)
{
	int i,j,route_count;
	int rip_count;
	char dotip1[60],dotip2[60];

	route_count = 0;
	for (i=0;i<ROUTE_MAX_NUM;i++)
	if( route_table[i].metric >= 1 )
	{
		route_count ++;
	}
	if (route_count  <= 0)
	{
		//print_message("warnning:", "the route table is empty");
		return;
	}

	//request for the whole route_table
	if (current_rip_count == 1 &&  current_rip_packet.rip_entries[0].family == 0 &&
		current_rip_packet.rip_entries[0].metric == 16)
	{
		
		for (i=0;i<ROUTE_MAX_NUM;)
		{
			rip_count = 0;
			for (j=i;j<ROUTE_MAX_NUM &&rip_count <RIP_MAX_ENTRY ;j++)
			if( route_table[j].metric >= 1 )
			{
				sprintf(dotip1,"%s",inet_ntoa(route_table[j].dest));
				sprintf(dotip2,"%s",inet_ntoa(route_table[j].next));
				if (strcmp(dotip1,srcip) &&strcmp(dotip2,srcip) &&route_table[j].valid)
				{
						current_rip_packet.rip_entries[rip_count ].ip.s_addr =route_table[j].dest.s_addr;
						current_rip_packet.rip_entries[rip_count ].metric = route_table[j].metric;
						rip_count ++;
				}
			}
			i = j;
			if (rip_count > 0)
			{
				current_rip_packet.command = RIP_RESPONSE;
				send_rip_packet(srcip);
			}
		}

		return;
	}
	//request for certain route entry

	for (i = 0; i < current_rip_count; i++)
	{
		for (j = 0; j < ROUTE_MAX_NUM; j++)
		{
			if (route_table[j].dest.s_addr == current_rip_packet.rip_entries[i].ip.s_addr &&route_table[j].metric > 0)
			{
				current_rip_packet.rip_entries[current_rip_count].ip.s_addr  = route_table[j].dest.s_addr;
				current_rip_packet.rip_entries[current_rip_count].metric  =route_table[j].metric;
				break;
			}
		}
		if(j == current_rip_count)
		{
			current_rip_packet.rip_entries[i].metric  = RIP_INFINITY;
		}
	}
	current_rip_packet.command = RIP_RESPONSE;

	send_rip_packet(srcip);
	return;
}


void print_routing_table()
{
	int i,rip_num;
	char 	buf[INET_ADDRSTRLEN];
	size_t	buf_len = sizeof(buf);

	rip_num = 0;
	for (i = 0; i < ROUTE_MAX_NUM; i++)
	if (route_table[i].metric >=1 )
	rip_num++;
	if (rip_num == 0)
	return;
	printf("Total RIP num:%d\n",rip_num);
	for (i = 0; i < ROUTE_MAX_NUM; i++)
	if (route_table[i].metric >=1 )
	{
		printf("%dth RIP:", i);
		inet_ntop(AF_INET, &route_table[i].dest, buf, buf_len);
		printf("destination: %s ", buf);
		inet_ntop(AF_INET, &route_table[i].next, buf, buf_len);
 		printf("next: %s ", buf);
		printf("metric: %d\n", route_table[i].metric);
	}
	printf("\n");
}


void print_rip_packet()
{
	return ;

	printf("          rip packet contains %d entries: \n", current_rip_count);
	printf("command: %s ",((current_rip_packet.command == RIP_RESPONSE)?"RESPONSE":"REQUEST"));
	printf("version: %d\n",current_rip_packet.version);
	for(int i = 0; i < current_rip_count; i++)
	{
		printf("entry %d: ",i);
		printf("ip: %s ",inet_ntoa(current_rip_packet.rip_entries[i].ip));
		printf("metric: %d\n",current_rip_packet.rip_entries[i].metric);
	}

}

int check_rip_packet()
{
	int i;
	if (current_rip_packet.version == 2)
		return RIP_CHECK_OK;
	if (current_rip_packet.version == 0)
		return RIP_CHECK_FAIL;
	if (current_rip_packet.zero)
		return RIP_CHECK_FAIL;
	for (i = 0; i < current_rip_count; i++)
 	{
		if (current_rip_packet.rip_entries[i].zero1 || current_rip_packet.rip_entries[i].zero2 ||
			current_rip_packet.rip_entries[i].zero3)
		{
			return RIP_CHECK_FAIL;
		}
		return RIP_CHECK_OK;
	}

	return -1;
}

int process_rip_packet(int packet_len, char *recv_buf,char *ip)
{
	int ret;
	
	memcpy((char*)&current_rip_packet,recv_buf,packet_len);
	current_rip_count = (packet_len - RIP_PACKET_HEAD) / sizeof(struct rip_entry);
	//inverse
	rp_inverse_ntoh();
	//end inverse
	print_rip_packet();
	ret = check_rip_packet();
	if (ret == RIP_CHECK_OK)
	switch(current_rip_packet.command)
	{
		case RIP_REQUEST:
			process_rip_request(ip);
			return 1;
		case RIP_RESPONSE:
			process_rip_response(ip);
			return 2;
		default:
			return -1;
	}
	else
	{
		print_message("error:", "packet check error");

		return -2;
	}

	return -3;
}


int judge_and_process_rip_packet(int recv_len ,char *recv_buff,char *srcip)
{
	int ret;
	short port_no;


	if (recv_len < 66)
	{//not RIP

		return 0;
	}
	memcpy((char*)&port_no,recv_buff+36, sizeof(short));
	//port_no = htons(recv_buff+36);
	port_no = htons(port_no );

	if (recv_buff[23] == 17 &&		//UDP
		port_no ==RIP_PORT )	//RIP packet
	{
		ret = is_local_ip_address(recv_buff+26);
		if (ret == 1)
		{//The rip packet come from local , ignore
			//ret = process_rip_packet(recv_len-42, recv_buff+42,srcip);//临时
			return 1;
		}
		ret = process_rip_packet(recv_len-42, recv_buff+42,srcip);
		return 1;
	}
	return 0;
}


void rip_process(int counter)
{
	int del_t;
	int i,j,rip_count;
	static time_t  last_time=0;
	time_t  cur_time;

	time(&cur_time);

	//sleep(2);
	del_t = cur_time - last_time_broadcast;
	//1. broadcast rip request
	if (counter == 0 || del_t >=1)
	{//first time 
		gen_rip_request_packet();
		broadcast_rip_request();
		time(&last_time_broadcast);
//printf("broadcast rip counter =%d,time=%d\n",counter,(int)last_time_broadcast);
	}
	
	update_routing_table();
	
	if ((cur_time -  last_time) >= (time_t)BROADCAST_INTERVAL )
	{
		//printf("begin broadcasting...\n");
		current_rip_packet.command = RIP_RESPONSE;
		current_rip_packet.version = 1;
		for (i=0;i<ROUTE_MAX_NUM;)
		{
			rip_count = 0;
			for (j=i;j<ROUTE_MAX_NUM &&rip_count <RIP_MAX_ENTRY ;j++)
			if( route_table[j].metric >= 1 )
			{
					current_rip_packet.rip_entries[rip_count ].ip.s_addr =route_table[j].dest.s_addr;
					current_rip_packet.rip_entries[rip_count ].metric = route_table[j].metric;
					rip_count ++;
			}
			i = j;
			if (rip_count > 0)
			{
				current_rip_packet.command = RIP_RESPONSE;
				broadcast_rip_request();
			}
		}

		last_time = cur_time ;
	}

}