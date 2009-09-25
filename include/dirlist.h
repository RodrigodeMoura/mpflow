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

DirList *new_DirList(void);
void destroy_DirList(DirList *);

void set_DirList(DirList *, char *);
void add_DirList(DirList *);

void add_directory(char *);

void init_current_DirList(void);
void move_DirList_left(void);
void move_DirList_right(void);

#endif	/* DIRLIST_H_WJ109 */

/* EOB */
