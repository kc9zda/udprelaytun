#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "udprelaytun.h"

char *cattun_loc;
int udpport;
struct sockaddr client_addr;
char *cattun_dev;
int cattun_stdin[2]={-1,-1};
int cattun_stdout[2]={-1,-1};
pid_t cattun_pid;
int udpfd;
int paired;
int client_addr_len;

void parse_args(int argc, char **argv) {
	if (argc < 4) {
		perror("invalid arguments");
		return;
		}
	cattun_dev = argv[1];
	udpport = atoi(argv[2]);
	cattun_loc = argv[3];
	}

void sig_handle(int signo) {
	if (signo == SIGINT) {
		kill(cattun_pid,SIGINT);
		close(udpfd);
		udpfd = -1;
		}
	}

void start_cattun() {
	execlp(cattun_loc, cattun_dev);
}

int parent_main() {
	struct sockaddr_in myaddr;
	socklen_t addrlen = sizeof(client_addr);

	if (signal(SIGINT, sig_handle) == SIG_ERR) {
		perror("cannot register signal handler");
		return -1;
		}
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(udpport);
	if (udpfd = alloc_server_udp_sock((struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("cannot create udp server socket");
		return -1;
		}
	fcntl(udpfd, F_SETFL, O_NONBLOCK);
	while (udpfd != -1) {
		relay_from_tun();
		relay_from_udp();
		}
}

int main(int argc, char **argv) {
	int cattun_stdin[2]={-1,-1};
	int cattun_stdout[2]={-1,-1};

#define PARENT_READ cattun_stdout[0]
#define CHILD_WRITE cattun_stdout[1]
#define CHILD_READ cattun_stdin[0]
#define PARENT_WRITE cattun_stdin[1]

	parse_args(argc,argv);

	if (pipe2(cattun_stdout,O_NONBLOCK) < 0 || pipe2(cattun_stdin,O_NONBLOCK)) {
		perror("cannot create pipe to cattun");
		return;
		}

	if ((cattun_pid = fork()) < 0) {
		perror("cannot fork to create cattun process");
		return;
		} else if (cattun_pid == 0) {
			close(PARENT_WRITE);
			close(PARENT_READ);
			dup2(CHILD_READ,0);
			close(CHILD_READ);
			dup2(CHILD_WRITE,1);
			close(CHILD_WRITE);

			start_cattun();
			} else {
				close(CHILD_READ);
				close(CHILD_WRITE);

				return parent_main();
				}
	}
