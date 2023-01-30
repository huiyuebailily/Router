#ifndef IP_LINK_H
#define IP_LINK_H

typedef struct ip_link{
	unsigned char ip[4];
	struct ip_link *next;
}IP_LINK;
extern IP_LINK *ip_head;
extern IP_LINK *inner_ip_link(IP_LINK *ip_head,IP_LINK* p);

extern void printf_ip_link(IP_LINK *ip_head);
extern IP_LINK *find_ip(IP_LINK *ip_head, unsigned char *ip);

extern void free_ip_link(IP_LINK *ip_head);
extern IP_LINK *del_ip_for_link(IP_LINK *head,unsigned char *ip);

extern void init_ip_link();
extern void save_ip_link();

#endif

