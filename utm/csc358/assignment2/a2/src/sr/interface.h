#ifndef INTERFACE_H
#define INTERFACE_H

#define MAX_INTERFACE_NUM 8    //max interface num /* 最大接口数 */

typedef struct net_interface{
	char name[20];
	unsigned char ip[4];
	unsigned char mac[6];
	unsigned char netmask[4];
	unsigned char br_ip[4];
	int  flag;
}	NET_INTERFACE;
extern NET_INTERFACE net_interface[MAX_INTERFACE_NUM ];	//interface infomation 

extern void init_interface();

int get_interface_num();


#endif
