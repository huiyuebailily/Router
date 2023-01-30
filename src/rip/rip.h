#ifndef _MYRIP_H
#define _MYRIP_H
#include <stdlib.h> 
#include <stdio.h>  
#include <unistd.h> 
#include <time.h> 
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



#define RIP_VERSION    	1 
#define RIP_REQUEST    	1
#define RIP_RESPONSE   	2
#define RIP_INFINITY  	16
#define RIP_MAX_ENTRY   25
#define RIP_MAX_PACKET  504
#define RIP_PACKET_HEAD	4



#define ROUTE_MAX_ENTRY 256
#define LOCAL_ROUTE_ENTRY 10
#define ROUTE_VALID	1
#define ROUTE_UNVALID	0

#define RIP_CHECK_OK	1
#define RIP_CHECK_FAIL	0

#define BROADCAST_INTERVAL	10
#define CLOCK_TIMEOUT		15
#define GARBAGE_REMOVE_TIMEOUT  25


struct rip_entry
{
	unsigned short family;
	unsigned short zero1;
	struct in_addr ip;
	unsigned int zero2;
	unsigned int zero3;
	unsigned int metric;
};

struct rip_packet
{

	unsigned char command;
	unsigned char version;
	unsigned short zero; 
	struct rip_entry rip_entries[RIP_MAX_ENTRY];
};

struct route_entry
{
	struct in_addr dest; 
	struct in_addr next;
	unsigned int metric;
	time_t timer;
	int valid;
	int flag;
};

struct rip_packet 	current_rip_packet;
int		current_rip_count;
struct route_entry	routing_table6[ROUTE_MAX_ENTRY];
int	sockfd;
int	broadcast_sock;
pthread_mutex_t rt_mutex;
pthread_mutex_t crp_mutex;
struct in_addr local_addr;


#endif
