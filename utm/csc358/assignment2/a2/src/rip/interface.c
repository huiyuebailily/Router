#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <netinet/ether.h>	
#include "interface.h"

//interface num

int interface_num ;

//Detail information for every interface
NET_INTERFACE net_interface[MAX_INTERFACE_NUM ];//interface infomation 

/******************************************************************
name:	int get_interface_num()
fuction:	return interface num
*******************************************************************/
int get_interface_num(){
	return interface_num;
}


/******************************************************************
name:	int init_interface()
function:	get information for every interface when start
*******************************************************************/
void init_interface()
{
	struct ifreq buf[MAX_INTERFACE_NUM ];
	struct ifconf ifc;                  	
	my_local_ip_address[0] = 0;
	//int retcode = gethostname(hostname, sizeof(hostname));
	//printf("This is %s\n",hostname);
	interface_num=0;
	int sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

	//1. init ifconf 
    	ifc.ifc_len = sizeof(buf);
   	 ifc.ifc_buf = (caddr_t) buf;
 
	//2.get list for interfaces
	if (ioctl(sock_raw_fd, SIOCGIFCONF, (char *) &ifc) == -1){
		perror("SIOCGIFCONF ioctl");
		return ;
	}
	interface_num = ifc.ifc_len / sizeof(struct ifreq); 
	printf("interface_num=%d\n\n", interface_num);

 	char buff[20]="";
	int ip;
	int if_len = interface_num;
	//3 get detail information for every interface
	int i = 0;
	while (if_len-- > 0){ 
		i++;
		//3. get interface name
		printf("%02dth interface:%s     ",i, buf[if_len].ifr_name);	//interface name
		sprintf(net_interface[if_len].name, "%s", buf[if_len].ifr_name);
		//printf("-%s--\n",net_interface[if_len].name);

		//3.2 get interface flag
		if (!(ioctl(sock_raw_fd, SIOCGIFFLAGS, (char *) &buf[if_len]))){
            			//inteeface state:	
			//	IFF_UP: in use
			//	IFF_DOWN: not use
			if (buf[if_len].ifr_flags & IFF_UP){
				//printf("UP    ");
				net_interface[if_len].flag = 1;
			}
			else{
				//printf("DOWN  ");
				net_interface[if_len].flag = 0;
			}
  		}else{
			//fail to get interface flag
			char str[256];
			sprintf(str, "SIOCGIFFLAGS ioctl %s", buf[if_len].ifr_name);
			perror(str);
		}
 
		//3.3 get IP address for the interface
		if (!(ioctl(sock_raw_fd, SIOCGIFADDR, (char *) &buf[if_len]))){
	
			printf("IP:%s    ",(char*)inet_ntoa(((struct sockaddr_in*) (&buf[if_len].ifr_addr))->sin_addr));
			bzero(buff,sizeof(buff));
			sprintf(buff, "%s", (char*)inet_ntoa(((struct sockaddr_in*) (&buf[if_len].ifr_addr))->sin_addr));
			inet_pton(AF_INET, buff, &ip);
			memcpy(net_interface[if_len].ip, &ip, 4);
		}else{
			//fail to get IP address
			char str[256];
			sprintf(str, "SIOCGIFADDR ioctl %s", buf[if_len].ifr_name);
			perror(str);
		}
 
		//3.4 get net mask
		if (!(ioctl(sock_raw_fd, SIOCGIFNETMASK, (char *) &buf[if_len]))){
			//printf("netmask:%s\n",(char*)inet_ntoa(((struct sockaddr_in*) (&buf[if_len].ifr_addr))->sin_addr));
			bzero(buff,sizeof(buff));
			sprintf(buff, "%s", (char*)inet_ntoa(((struct sockaddr_in*) (&buf[if_len].ifr_addr))->sin_addr));
			inet_pton(AF_INET, buff, &ip);
			memcpy(net_interface[if_len].netmask, &ip, 4);
		}else{
			//fail to get net mask
			char str[256];
			sprintf(str, "SIOCGIFADDR ioctl %s", buf[if_len].ifr_name);
			perror(str);
		}
 
		//3.5 get broadcast address
		if (!(ioctl(sock_raw_fd, SIOCGIFBRDADDR, (char *) &buf[if_len]))){
			//printf("	br_ip:%s    ",(char*)inet_ntoa(((struct sockaddr_in*) (&buf[if_len].ifr_addr))->sin_addr));
			bzero(buff,sizeof(buff));
			sprintf(buff, "%s", (char*)inet_ntoa(((struct sockaddr_in*) (&buf[if_len].ifr_addr))->sin_addr));
			inet_pton(AF_INET, buff, &ip);
			memcpy(net_interface[if_len].br_ip, &ip, 4);
		}else{
			char str[256];
			sprintf(str, "SIOCGIFADDR ioctl %s", buf[if_len].ifr_name);
 			perror(str);
		}

		//3.6 get MAC address
		/*MACµÿ÷∑ */
		if (!(ioctl(sock_raw_fd, SIOCGIFHWADDR, (char *) &buf[if_len]))){
			printf("MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",
				(unsigned char) buf[if_len].ifr_hwaddr.sa_data[0],
				(unsigned char) buf[if_len].ifr_hwaddr.sa_data[1],
				(unsigned char) buf[if_len].ifr_hwaddr.sa_data[2],
				(unsigned char) buf[if_len].ifr_hwaddr.sa_data[3],
				(unsigned char) buf[if_len].ifr_hwaddr.sa_data[4],
				(unsigned char) buf[if_len].ifr_hwaddr.sa_data[5]);
			memcpy(net_interface[if_len].mac, (unsigned char *)buf[if_len].ifr_hwaddr.sa_data, 6);
		}else{
			char str[256];
			sprintf(str, "SIOCGIFHWADDR ioctl %s", buf[if_len].ifr_name);
			perror(str);
		}
	}

	close(sock_raw_fd);   //close the raw socket
}

int  is_local_ip_address(char *ip_address)
{

	for (int i=0;i<interface_num;i++)
	if (ip_address[0] == net_interface[i].ip[0] && ip_address[1] == net_interface[i].ip[1] && 
		ip_address[2] == net_interface[i].ip[2] && ip_address[3] == net_interface[i].ip[3])
	{
		return 1;
	}

	return 0;
}