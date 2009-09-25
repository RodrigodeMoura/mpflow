/*
	dirlist.c	WJ109
*/

#include "dirlist.h"
#include "mpdconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


const char *album_art[] = {
	"cover.jpg",
	"Folder.jpg",
	"AlbumArt.jpg"
};

const char *no_album_art = "";

DirList *root_dirlist = NULL;


DirList *new_DirList(void) {
DirList *d;

	if ((d = (DirList *)malloc(sizeof(DirList))) == NULL)
		return NULL;

	memset(d, 0, sizeof(DirList));
	return d;
}

void destroy_DirList(DirList *d) {
	if (d->path != NULL)
		free(d->path);

	if (d->name != NULL)
		free(d->name);

/* d->img is not allocated */

	free(d);
}

/*
	set dirlist to point to path
*/
void set_DirList(DirList *d, char *path) {
char *p;

	if ((d->path = strdup(path)) == NULL) {
		fprintf(stderr, "out of memory\n");
		return;
	}
	if ((p = strrchr(path, '/')) == NULL)
		p = path;
	else {
		p++;
		if (!*p)
			p = path;
	}
	if ((d->name = strdup(p)) == NULL) {
		fprintf(stderr, "out of memory\n");
		return;
	}
	for(p = d->name; *p; p++) {
		if (*p == '_')
			*p = ' ';
	}
}

void add_DirList(DirList *d) {
	if (root_dirlist == NULL)
		root_dirlist = d;
	else {
		d->next = root_dirlist;
		root_dirlist->prev = d;
		root_dirlist = d;
	}
}

void add_directory(char *path) {
DirList *d;

	if ((d = new_DirList()) == NULL) {
		fprintf(stderr, "out of memory\n");
		return;
	}
	set_DirList(d, path);
	add_DirList(d);
}

/*
	see if album art files exist
*/
void find_album_art(DirList *d) {
char filename[1024];
struct stat statbuf;
int i, num;

	if (d->img != NULL)
		return;

	num = sizeof(album_art) / sizeof(char *);

	for(i = 0; i < num; i++) {
		snprintf(filename, sizeof(filename), "%s/%s/%s", config_musicdir, d->path, album_art[i]);

		if (!stat(filename, &statbuf) && S_ISREG(statbuf.st_mode)) {
			d->img = album_art[i];
			printf("TD %s\n", filename);
			return;
		}
	}
	d->img = no_album_art;
}

/* EOB */
