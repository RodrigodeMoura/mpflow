/*
	mpdconf.c	WJ109
*/

#include "mpdconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *mpd_conf = "/etc/mpd.conf";
char *config_musicdir = NULL;
char *config_address = "localhost";
char *config_port = "6600";
char *config_password = NULL;


/*
	read the MPD config file
*/
int read_mpdconf(void) {
FILE *f;
char buf[1280], *p;
int lineno, len;

	if ((f = fopen(mpd_conf, "rb")) == NULL) {
		fprintf(stderr, "error reading config file '%s'\n", mpd_conf);
		return -1;
	}
	lineno = 0;
	while(fgets(buf, sizeof(buf), f) != NULL) {
		lineno++;

		if (buf[0] == '#')
			continue;

/* strip line */
		len = strlen(buf)-1;
		while(len >= 0 && (buf[len] == '\r' || buf[len] == '\n' || buf[len] == ' ' || buf[len] == '\t'))
			buf[len--] = 0;

/* buf starts with directive, let p be the value */
		if ((p = strchr(buf, ' ')) == NULL && (p = strchr(buf, '\t')) == NULL)
			continue;

		*p = 0;
		p++;

		if (!*p)
			continue;

		while(*p == ' ' || *p == '\t')
			p++;

		if (*p == '"')
			p++;

		len = strlen(p)-1;
		if (p[len] == '"')
			p[len--] = 0;

/* directive: music_directory "/path/dir" */
		if (!strcmp(buf, "music_directory")) {
			if (!*p) {
				fprintf(stderr, "%s:%d: expected a directory\n", mpd_conf, lineno);
				fclose(f);
				return -1;
			}
			if ((config_musicdir = strdup(p)) == NULL) {
				fprintf(stderr, "out of memory\n");
				fclose(f);
				return -1;
			}
		}
/* bind_address "localhost" */
		if (!strcmp(buf, "bind_address")) {
			if (!*p) {
				fprintf(stderr, "%s:%d: expected an interface address, like \"localhost\"\n", mpd_conf, lineno);
				fclose(f);
				return -1;
			}
			if ((config_address = strdup(p)) == NULL) {
				fprintf(stderr, "out of memory\n");
				fclose(f);
				return -1;
			}
		}
/* port to connect to (MPD listens at this port) */
		if (!strcmp(buf, "port")) {
			if (!*p) {
				fprintf(stderr, "%s:%d: expected a port number\n", mpd_conf, lineno);
				fclose(f);
				return -1;
			}
			if ((config_port = strdup(p)) == NULL) {
				fprintf(stderr, "out of memory\n");
				fclose(f);
				return -1;
			}
		}
/* mpd passworded access */
		if (!strcmp(buf, "password")) {
			char *p2;

			if (!*p) {
				fprintf(stderr, "%s:%d: expected a password\n", mpd_conf, lineno);
				fclose(f);
				return -1;
			}
			if ((p2 = strchr(p, '@')) != NULL)
				*p2 = 0;

			if ((config_password = strdup(p)) == NULL) {
				fprintf(stderr, "out of memory\n");
				fclose(f);
				return -1;
			}
		}
	}
	fclose(f);

	printf("TD config_musicdir %s\n", config_musicdir);
	printf("TD config_address %s\n", config_address);
	printf("TD config_port %s\n", config_port);
	printf("TD config_password %s\n", config_password);
	return 0;
}

void set_conffile(char *filename) {
	if (filename != NULL && *filename) {
		char *p;

		if ((p = strdup(filename)) == NULL)
			fprintf(stderr, "odd, out of memory\n");
		else
			mpd_conf = p;
	}
}

/* EOB */
