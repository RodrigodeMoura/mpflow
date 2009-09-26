/*
	mpd.c	WJ109
*/

#include "mpd.h"
#include "mpdconf.h"
#include "inet.h"
#include "dirlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int mpd_connect(void) {
int sock;
char buf[1280], *rest;

	if ((sock = inet_connect(config_address, config_port)) == -1)
		return -1;

/* MPD announces itself; read first line */
	rest = NULL;
	if (inet_readline(sock, buf, &rest, sizeof(buf)) == NULL) {
		fprintf(stderr, "error in connection to MPD\n");
		inet_close(sock);
		return -1;
	}
	if (strncmp(buf, "OK MPD ", 7)) {
		fprintf(stderr, "error: the service we connected to does not look like MPD\n");
		inet_close(sock);
		return -1;
	}
/* authenticate */
	if (config_password != NULL) {
		snprintf(buf, sizeof(buf), "password %s\n", config_password);
		inet_write(sock, buf);

		rest = NULL;
		if (inet_readline(sock, buf, &rest, sizeof(buf)) == NULL) {
			fprintf(stderr, "error in connection to MPD\n");
			inet_close(sock);
			return -1;
		}
		if (strncmp(buf, "OK", 2)) {
			fprintf(stderr, "error: wrong password for MPD\n");
			inet_close(sock);
			return -1;
		}
	}
	return sock;
}

/*
	get all directories from mpd
*/
int mpd_listdir(int sock) {
char buf[1280], *rest;

	inet_write(sock, "listall\n");

	rest = NULL;
	while(inet_readline(sock, buf, &rest, sizeof(buf)) != NULL) {
		if (!strncmp(buf, "directory: ", 11)) {
			add_directory(buf+11);
			continue;
		}
		if (!strcmp(buf, "OK"))
			break;

		if (!strncmp(buf, "ACK", 3)) {
			fprintf(stderr, "error from MPD: error listing music directory\n");
			return -1;
		}
	}
	init_current_DirList();
	return 0;
}

void mpd_close(int sock) {
	inet_write(sock, "close\n");
	inet_close(sock);
}

/* EOB */
