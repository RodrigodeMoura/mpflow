/*
	inet.c	WJ109
*/

#include "inet.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int inet_connect(char *addr, char *service) {
int sock, err;
struct addrinfo hints, *result, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((err = getaddrinfo(addr, service, &hints, &result)) != 0) {
		fprintf(stderr, "error: getaddrinfo(): %s\n", gai_strerror(err));
		return -1;
	}
/*
	the result set is a list of address
	if a connect fails, try to connect to the next one (IPv6 may or may not be enabled ...)
*/
	for(rp = result; rp != NULL; rp = rp->ai_next) {
		if ((sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
			continue;

		if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
			break;

		close(sock);
		sock = -1;
	}
	freeaddrinfo(result);

	if (sock == -1) {
		fprintf(stderr, "failed to connect to %s:%s\n", addr, service);
		return -1;
	}

	return sock;
}

void inet_close(int sock) {
	shutdown(sock, SHUT_RDWR);
	close(sock);
}

/*
	read line from network
	- sock is socket fd
	- buf is buffer
	- restp is address of pointer to rest data in buffer, what was left after the line
	- size is max size of buffer
*/
char *inet_readline(int sock, char *buf, char **restp, int size) {
char *rest;
int len;

	if (sock < 0 || size <= 0 || buf == NULL || restp == NULL)
		return NULL;

	rest = *restp;
	if (rest != NULL && *rest) {
		len = strlen(rest);
		memmove(buf, rest, len+1);
		*restp = NULL;
	} else {
		*buf = 0;
		len = 0;
	}
	if ((rest = strchr(buf, '\n')) == NULL) {
		int err;

		if ((err = read(sock, buf + len, size - (len+1))) <= 0)
			return NULL;

		buf[len + err] = 0;
		len = strlen(buf);

		if ((rest = strchr(buf, '\n')) == NULL) {
			*restp = NULL;
			return buf;
		}
	}
	*rest = 0;
	rest++;
	*restp = rest;
	return buf;
}

int inet_write(int sock, char *str) {
	return write(sock, str, strlen(str));
}

/* EOB */
