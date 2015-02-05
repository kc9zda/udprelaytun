#include <string.h>
#include <sys/socket.h>

#include "udprelaytun.h"

void relay_from_tun(void) {
	int szfr;
	unsigned char buf[1500];

	if (paired != 0) {
		read(PARENT_READ, &szfr, sizeof(int));
		read(PARENT_READ, buf, 1500);
		sendto(udpfd,buf,szfr,0,(struct sockaddr *)&client_addr, client_addr_len);
		}
	}

void relay_from_udp(void) {
	int recvlen;
	unsigned char buf[1500];
	struct sockaddr from;
	int from_len;

	recvlen = recvfrom(udpfd,buf,1500,0,&from,&from_len);
	if (recvlen == 1 && buf[0] == 0x9f) {
		memset(&buf,0,1500);
		buf[0] = 0xa8;
		memcpy(&client_addr, &from, from_len);
		client_addr_len = from_len;
		sendto(udpfd,buf,1,0,&client_addr,client_addr_len);
		paired = 1;
		printf("paired with client");
		}
	if (paired != 0) {
		if (sockaddrcmp(&client_addr,client_addr_len,&from,from_len)) {
			write(PARENT_WRITE,&recvlen,sizeof(int));
			write(PARENT_WRITE,buf,recvlen);
			printf("recvd frame from UDP: %d bytes", recvlen);
			}
		}
	}
