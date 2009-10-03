/*
	cover.h	WJ109
*/

#ifndef COVER_H_WJ109
#define COVER_H_WJ109	1

#include "SDL.h"
#include "main.h"
#include "dirlist.h"

#define CENTER_COVER	6
#define NUM_COVERS		(CENTER_COVER*2 + 1)

#define COVER_W			140
#define COVER_H			COVER_W
#define COVER_Z			-50.0f
#define COVER_ANGLE		60.0f
#define COVER_DISTANCE	(COVER_W * 0.125f)
#define CENTER_SPACE	(COVER_W * 0.75f)

#define FLIP_SPEED		(FPS * 0.2f)
#define FLIP_SPEED_FAST	(FPS * 0.06f)
#define SPEEDUP_DELAY	1600

typedef struct {
	float x, y, z, angle, color;
	int pos, texture_idx;
	DirList *dirlist;
} Cover;

extern Cover covers[NUM_COVERS];
extern SDL_Rect center_cover;

void init_covers(void);
void move_covers_left(void);
void move_covers_right(void);
void draw_cover(Cover *);
void draw_title(char *);
void draw_covers(void);
void startup_cover(void);

#endif	/* COVER_H_WJ109 */

/* EOB */
