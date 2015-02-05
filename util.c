#include "udprelaytun.h"

int sockaddrcmp(struct sockaddr *a, int a_len, struct sockaddr *b, int b_len) {
	if (a_len != b_len) return -1;
	else {
		return memcmp(a, b, a_len);
		}
	return -1;
	}
