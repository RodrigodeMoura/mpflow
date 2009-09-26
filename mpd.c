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

		if (inet_write(sock, buf) == -1) {
			fprintf(stderr, "error in connection with MPD\n");
			return -1;
		}
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

	if (inet_write(sock, "listall\n") == -1) {
		fprintf(stderr, "error in connection with MPD\n");
		return -1;
	}
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

int mpd_command(int sock, char *command) {
char buf[256], *rest;

	if (inet_write(sock, command) == -1) {
		fprintf(stderr, "error in connection with MPD\n");
		return -1;
	}
	rest = NULL;
	while(inet_readline(sock, buf, &rest, sizeof(buf)) != NULL) {
		if (!strcmp(buf, "OK"))
			break;

		if (!strncmp(buf, "ACK", 3)) {
			fprintf(stderr, "error from MPD: error listing music directory\n");
			return -1;
		}
	}
	return 0;
}

int mpd_play(int sock, char *entry) {
char buf[1280];

	if (mpd_command(sock, "clear\n") == -1)
		return -1;

	snprintf(buf, sizeof(buf), "add \"%s\"\n", entry);

	if (mpd_command(sock, buf) == -1)
		return -1;

	if (mpd_command(sock, "play\n") == -1)
		inet_close(sock);

	return 0;
}

int play_album(char *album) {
int sock;

	if ((sock = mpd_connect()) == -1)
		return -1;

	mpd_play(sock, album);

	mpd_close(sock);
	return 0;
}

int mpd_current_album(int sock, char *album, int size) {
char buf[1280], *rest, *p;

	if (album == NULL || size < 1)
		return -1;

	*album = 0;

/* try currently playing song */
	if (inet_write(sock, "currentsong\n") == -1) {
		fprintf(stderr, "error in connection with MPD\n");
		return -1;
	}
	rest = NULL;
	while(inet_readline(sock, buf, &rest, sizeof(buf)) != NULL) {
		if (!strncmp(buf, "file: ", 6)) {
			if ((p = strrchr(buf, '/')) != NULL)
				*p = 0;

			strncpy(album, buf+6, size-1);
			album[size-1] = 0;
		}
		if (!strcmp(buf, "OK"))
			break;

		if (!strncmp(buf, "ACK", 3))
			break;
	}
	if (*album)					/* found */
		return 0;

/* try album intop of playlist */
	if (inet_write(sock, "playlist\n") == -1) {
		fprintf(stderr, "error in connection with MPD\n");
		return -1;
	}
	rest = NULL;
	while(inet_readline(sock, buf, &rest, sizeof(buf)) != NULL) {
		if (buf[0] == '0' && buf[1] == ':') {			/* use first line only */
			if ((p = strrchr(buf, '/')) != NULL)
				*p = 0;

			strncpy(album, buf+2, size-1);
			album[size-1] = 0;
		}
		if (!strcmp(buf, "OK"))
			break;

		if (!strncmp(buf, "ACK", 3)) {
			fprintf(stderr, "error from MPD: error listing playlist\n");
			return -1;
		}
	}
	return 0;
}

/* EOB */
