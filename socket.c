#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>  
#include <netinet/udp.h> 
#include <string.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>

#include "socket.h"
#include "tun.h"
#include "network.h"

//static int raw_fd;
static int send6_fd;
//static char buf[2000];

char buf[BUF_LEN];
char buf4[BUF_LEN];

static int raw_fd;

static struct sockaddr_in6 dest;

int socket_init()
{
	raw_fd = socket(AF_INET6, SOCK_RAW, IPPROTO_IPIP);
	if (raw_fd < 0) {
		//fprintf(stderr, "socket_init: Error Creating socket: %m\n", errno);
		return -1;
	}
	struct sockaddr_in6 src;
	memset(&src, 0, sizeof(src));
	src.sin6_family = AF_INET6;
	memcpy(&src.sin6_addr, &(addr6_TI), sizeof(struct in6_addr));
	bind(raw_fd, (struct sockaddr *)&src, sizeof(src));
	return raw_fd;
}

int socket_init_tun()
{
	//send6_fd = socket(PF_INET6, SOCK_RAW, IPPROTO_RAW);
	send6_fd = socket(AF_INET6, SOCK_RAW, IPPROTO_IPIP);
	if (send6_fd < 0) {
		fprintf(stderr, "socket_init: Error Creating send socket: \n");
		return -1;
	}
	struct sockaddr_in6 src;
	memset(&src, 0, sizeof(src));
	src.sin6_family = AF_INET6;
	memcpy(&src.sin6_addr, &(addr6_TI), sizeof(struct in6_addr));
	bind(send6_fd, (struct sockaddr *)&src, sizeof(src));
	
	memset(&dest, 0, sizeof(dest));
	dest.sin6_family = AF_INET6;
	memcpy(&dest.sin6_addr, &(addr6_TC), sizeof(struct in6_addr));
	return send6_fd;
}

int handle_socket()
{
	struct sockaddr_in6 sin6addr;
	socklen_t addr_len = sizeof (sin6addr);
	int len = recvfrom(raw_fd, buf4, BUF_LEN, 0, (struct sockaddr*)&sin6addr, &addr_len);
	//send4_len = len;
	if (len < 0)
		return -1;

	tun_send(buf4, len);
	return 0;
}

int socket_send(char *buf, int len)
{
	if (sendto(send6_fd, buf, len, 0, (struct sockaddr *)&dest, sizeof(dest)) != len) {
		//fprintf(stderr, "socket_send: Failed to send ipv6 packet len=%d: \n", len);
		return -1;
	}
	return 0;
}
