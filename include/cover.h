/*
	cover.h	WJ109
*/

#ifndef COVER_H_WJ109
#define COVER_H_WJ109	1

#define NUM_COVERS		11

#define COVER_W			140
#define COVER_H			COVER_W

typedef struct {
	float x, y, z, angle, color;
	int texture_idx;
} Cover;

extern Cover covers[NUM_COVERS];

void init_covers(void);
void draw_covers(void);

#endif	/* COVER_H_WJ109 */

/* EOB */
