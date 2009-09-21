/*
	cover.h	WJ109
*/

#ifndef COVER_H_WJ109
#define COVER_H_WJ109	1

#define CENTER_COVER	6
#define NUM_COVERS		(CENTER_COVER*2 + 1)

#define COVER_W			140
#define COVER_H			COVER_W
#define COVER_Z			-50.0f
#define COVER_ANGLE		60.0f
#define COVER_DISTANCE	(COVER_W * 0.125f)
#define CENTER_SPACE	(COVER_W * 0.75f)

typedef struct {
	float x, y, z, angle, color;
	int pos, texture_idx;
} Cover;

extern Cover covers[NUM_COVERS];

void init_covers(void);
void move_covers_left(void);
void move_covers_right(void);
void draw_covers(void);

#endif	/* COVER_H_WJ109 */

/* EOB */
