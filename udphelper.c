#include <sys/socket.h>

#include "udprelaytun.h"

int alloc_server_udp_sock(struct sockaddr *bindaddr, int szbindaddr) {
	int fd = -1;
	int rv;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create udp socket");
		return fd;
		}
	if (rv = bind(fd, bindaddr, szbindaddr) < 0) {
		perror("udp bind failed");
		return rv;
		}
	return fd;
}
