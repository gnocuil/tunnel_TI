#include <linux/if_tun.h>
#include <net/if.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <netinet/ip6.h>  
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <fcntl.h>

#include "tun.h"
#include "socket.h"
#include "network.h"

static int tun_fd;

struct in6_addr addr6_TI, addr6_TC;

int tun_create(char *dev)
{
	struct ifreq ifr;
	int err;

	if ((tun_fd = open("/dev/net/tun", O_RDWR)) < 0) {
		fprintf(stderr, "tun_create: Error Creating TUN/TAP:\n");
		return -1;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags |= IFF_TUN | IFF_NO_PI;

	if (*dev != '\0') {
		strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	}

	if ((err = ioctl(tun_fd, TUNSETIFF, (void *)&ifr)) < 0) {
		fprintf(stderr, "tun_create: Error Setting tunnel name %s:\n", dev);
		close(tun_fd);
		return -1;
	}
/*
	if (fcntl(tun_fd, F_SETFL, O_NONBLOCK) < 0) {
		fprintf(stderr, "tun_create: Error Setting nonblock: %s\n", dev);
		return -1;
	}
*/
	
	strcpy(dev, ifr.ifr_name);
	
	return tun_fd;
}

int tun_send(char *packet, int len)
{
	int count = write(tun_fd, packet, len);
	/*
	if (count != len) {
		//fprintf(stderr, "tun_send : Error sending len=%d count=%d\n", len, count);
		return -1;
	}*/
	return 0;
}

int handle_tun()
{
	int len = read(tun_fd, buf, BUF_LEN);
	
	if (len < 0)
		return -1;
	return socket_send(buf, len);
}
