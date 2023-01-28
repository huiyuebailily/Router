#ifndef ARP_H
#define ARP_H

typedef struct arp_struct{
	unsigned char ip[4];		//ip address
	unsigned char mac[6];		//mac address
	struct arp_struct *next;
}ARP_STRUCT;
extern ARP_STRUCT *arp_head;

extern ARP_STRUCT *inner_arp_link(ARP_STRUCT *arp_head,ARP_STRUCT * p);

extern void printf_arp_link(ARP_STRUCT *arp_head);

extern ARP_STRUCT *find_arp_from_ip(ARP_STRUCT *arp_head, unsigned char *ip);
extern void free_arp_list(ARP_STRUCT *arp_head);

extern void *arp_packet_process(void *arg);

extern void *transfer_data(void *arg);



#endif
