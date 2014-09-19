#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <linux/if_tun.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tun.h"
#include "network.h"
#include "socket.h"

#define DEFAULT_MTU 1460

static void usage()
{
	fprintf(stderr, "Usage: tunnel [options] <TI_IPv6_ADDR> <TC_IPv6_ADDR>\n");
	fprintf(stderr, "  options: --name <TUNNEL_NAME>       default: 4over6\n");
	fprintf(stderr, "           --mtu <MTU_VALUE>          default: %d\n", DEFAULT_MTU);
	
	exit(1);
}

static void setDaemon()
{
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    umask(0);       
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
        
    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log any failure here */
        exit(EXIT_FAILURE);
    }
        
        /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	char tun_name[IFNAMSIZ] = {0};
	strncpy(tun_name, TUNNEL_NAME, IFNAMSIZ);
	mtu = DEFAULT_MTU;
	
	if (argc < 3)
		usage();
    int i;
	for (i = 1; i < argc - 2; ++i) {
		if (strcmp(argv[i], "--help") == 0) {
			usage();
		}
		if (i + 1 < argc - 2 && strcmp(argv[i], "--name") == 0) {
			strncpy(tun_name, argv[++i], IFNAMSIZ);
		} else if (i + 1 < argc - 2 && strcmp(argv[i], "--mtu") == 0) {
			++i;
			sscanf(argv[i], "%d", &mtu);
		}
	}
	printf("TI_IPv6_ADDR: %s\nTC_IPv6_ADDR: %s\n", argv[argc - 2], argv[argc - 1]);
	inet_pton(AF_INET6, argv[argc - 2], &addr6_TI);
	inet_pton(AF_INET6, argv[argc - 1], &addr6_TC);
	
	//Create TUN/TAP interface
	int tun_fd = tun_create(tun_name);
	if (tun_fd < 0) {
		exit(1);
	}
	fprintf(stderr, "interface name: %s\n", tun_name);

	set_mtu(tun_name, mtu);//set mtu
	interface_up(tun_name);//interface up
	
	setDaemon();

	int pid = fork();
	if (pid == 0) {//son
		int raw_fd = socket_init();
		if (raw_fd < 0) {
			exit(1);
		}
		while (1) {
			handle_socket();
        }
	}
	//father
	socket_init_tun();

	while (1) {
        handle_tun();
	}

	return 0;
}
