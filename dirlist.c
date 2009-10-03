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
	"AlbumArtSmall.jpg",
	NULL
};

const char *no_album_art = "";

static DirList *root_dirlist = NULL;


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
		d->prev = root_dirlist;
		root_dirlist->next = d;
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
int i;

	if (d->img != NULL)
		return;

	i = 0;
	while(album_art[i] != NULL) {
		snprintf(filename, sizeof(filename), "%s/%s/%s", config_musicdir, d->path, album_art[i]);

		if (!stat(filename, &statbuf) && S_ISREG(statbuf.st_mode)) {
			d->img = album_art[i];
			return;
		}
		i++;
	}
	d->img = no_album_art;
}

void init_current_DirList(void) {
DirList *d;
int n;

	n = 0;
	for(d = root_dirlist; d != NULL; d = d->prev) {
		n++;

/* make cycling chain */
		if (d->prev == NULL) {
			d->prev = root_dirlist;
			root_dirlist->next = d;
			break;
		}
	}
/* center the cursor */
	n /= 2;
	while(n > 0) {
		root_dirlist = root_dirlist->next;
		n--;
	}
}

DirList *find_dirlist(char *path) {
DirList *p;

	if (path == NULL || !*path)
		return NULL;

	if (!strcmp(root_dirlist->path, path))
		return root_dirlist;

	if (root_dirlist->next == root_dirlist)
		return NULL;

	for(p = root_dirlist->next; p != root_dirlist; p = p->next) {
		if (!strcmp(path, p->path)) {
			root_dirlist = p;
			return p;
		}
	}
	return NULL;
}

DirList *get_dirlist(void) {
	return root_dirlist;
}

void set_root_dirlist(DirList *p) {
	root_dirlist = p;
}

/* EOB */
