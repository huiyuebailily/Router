#ifndef ARP_H
#define ARP_H

#define ROUTE_MAX_NUM 256


struct route_struct
{
	unsigned char ip[4];		//(char *)IP address ,just for local===
	unsigned char mac[6];		//(char *)mac address ,just fo local=== 
	
	//char	inuse;			//inuse=0, this entry no use; inuse=1, in use
	//char	local;			//local >1, come from other routers

	struct in_addr dest; 		//IP address
	struct in_addr next;			//Next IP on route
	int metric;			//=-1 no used; =1, local ; >1 remote ===
	time_t time;			//===
	int valid;				//1: renew <CLOCK_TIMEOUT; 0,CLOCK_TIMEOUT<renew <GARBAGE_REMOVE_TIMEOUT=== 
	int flag;				//1:trigger ===
};

extern  struct route_struct route_table[ROUTE_MAX_NUM];

extern void *arp_packet_process(void *arg);
extern void *transfer_data(void *arg);



#endif
