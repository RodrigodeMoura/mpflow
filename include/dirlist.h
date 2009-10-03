/*
	dirlist.h	WJ109
*/

#ifndef DIRLIST_H_WJ109
#define DIRLIST_H_WJ109	1

typedef struct DirList_tag DirList;

struct DirList_tag {
	DirList *prev, *next;
	char *path, *name;
	const char *img;
};

extern const char *album_art[];

DirList *new_DirList(void);
void destroy_DirList(DirList *);

void set_DirList(DirList *, char *);
void add_DirList(DirList *);

void add_directory(char *);

void find_album_art(DirList *);
void init_current_DirList(void);
DirList *find_dirlist(char *);
DirList *get_dirlist(void);
void set_root_dirlist(DirList *);

#endif	/* DIRLIST_H_WJ109 */

/* EOB */
