/*
	cover.h	WJ109
*/

#ifndef COVER_H_WJ109
#define COVER_H_WJ109	1

#define CENTER_COVER	5
#define NUM_COVERS		(CENTER_COVER*2 + 1)

#define COVER_W			140
#define COVER_H			COVER_W

typedef struct {
	float x, y, z, angle, color;
	int texture_idx;
} Cover;

extern Cover covers[NUM_COVERS];

void init_covers(void);
void move_cover_left(void);
void move_cover_right(void);
void draw_covers(void);

#endif	/* COVER_H_WJ109 */

/* EOB */
