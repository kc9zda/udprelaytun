#include <sys/socket.h>

char *cattun_loc;
int udpport;
struct sockaddr client_addr;
char *cattun_dev;
int udpfd;
int paired;
int cattun_stdin[2];
int cattun_stdout[2];
int client_addr_len;

#define PARENT_READ cattun_stdout[0]
#define CHILD_WRITE cattun_stdout[1]
#define CHILD_READ cattun_stdin[0]
#define PARENT_WRITE cattun_stdin[1]

void parse_args(int argc, char **argv);
void start_cattun();
int parent_main();
int main(int argc, char **argv);
int alloc_server_udp_sock(struct sockaddr *bindaddr, int szbindaddr);
void relay_from_tun(void);
void relay_from_udp(void);
int sockaddrcmp(struct sockaddr *a, int a_len, struct sockaddr *b, int b_len);
