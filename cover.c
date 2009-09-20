/*
	cover.c	WJ109
*/

#include "cover.h"
#include "SDK.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>


Cover covers[NUM_COVERS];

extern GLuint textures[11];


void init_covers(void) {
int i;
float xpos, ypos, color;

/* left side */
	xpos = ARENA_WIDTH * 0.5f - CENTER_SPACE - 5 * COVER_DISTANCE;
	ypos = ARENA_HEIGHT * 0.5f;
	color = 0.5f;

	covers[0].x = xpos;
	covers[0].y = ypos;
	covers[0].z = COVER_Z;
	covers[0].angle = COVER_ANGLE;
	covers[0].color = color;
	covers[0].texture_idx = 0;

	for(i = 1; i < CENTER_COVER; i++) {
		covers[i].x = xpos;
		covers[i].y = ypos;
		covers[i].z = COVER_Z;
		covers[i].angle = COVER_ANGLE;
		covers[i].color = color;
		covers[i].texture_idx = 0;

		xpos += COVER_DISTANCE;
		color += 0.1f;
	}

/* right side */
	xpos = ARENA_WIDTH * 0.5f + CENTER_SPACE + 5 * COVER_DISTANCE;
	color = 0.5f;

	covers[NUM_COVERS-1].x = xpos;
	covers[NUM_COVERS-1].y = ypos;
	covers[NUM_COVERS-1].z = COVER_Z;
	covers[NUM_COVERS-1].angle = -COVER_ANGLE;
	covers[NUM_COVERS-1].color = color;
	covers[NUM_COVERS-1].texture_idx = 0;

	for(i = NUM_COVERS-2; i > CENTER_COVER; i--) {
		covers[i].x = xpos;
		covers[i].y = ypos;
		covers[i].z = COVER_Z;
		covers[i].angle = -COVER_ANGLE;
		covers[i].color = color;
		covers[i].texture_idx = 0;

		xpos -= COVER_DISTANCE;
		color += 0.1f;
	}

/* center cover */
	covers[CENTER_COVER].x = ARENA_WIDTH * 0.5f;
	covers[CENTER_COVER].y = ypos;
	covers[CENTER_COVER].z = 0;
	covers[CENTER_COVER].angle = 0;
	covers[CENTER_COVER].color = 1;
	covers[CENTER_COVER].texture_idx = 0;
}

void insert_cover_left(void) {
	;
}

void insert_cover_right(void) {
int i, tex;

/* TODO: delete texture that was on the left */

	tex = covers[0].texture_idx;

	for(i = 1; i < NUM_COVERS; i++) {
		covers[i-1].x = covers[i].x;
		covers[i-1].y = covers[i].y;
		covers[i-1].z = covers[i].z;
		covers[i-1].angle = covers[i].angle;
		covers[i-1].color = covers[i].color;
		covers[i-1].texture_idx = covers[i].texture_idx;
	}
	covers[NUM_COVERS-1].x = ARENA_WIDTH * 0.5f + CENTER_SPACE + 5 * COVER_DISTANCE;
	covers[NUM_COVERS-1].texture_idx = tex;
}

void move_cover_left(void) {
float xpos, step_x, step_z, step_angle, step_color, speed;
int i;

	speed = FPS * 0.5f;
	step_x = COVER_DISTANCE / speed;
	step_color = 0.1f / speed;

	for(i = 1; i < CENTER_COVER; i++) {
		covers[i].x -= step_x;
		covers[i].color -= step_color;
	}
	if (covers[1].x < covers[0].x)
		covers[1].x = covers[0].x;

	for(i = CENTER_COVER+1; i < NUM_COVERS-1; i++) {
		covers[i].x -= step_x;
		covers[i].color += step_color;
	}
	if (covers[NUM_COVERS-2].x > covers[NUM_COVERS-1].x)
		covers[NUM_COVERS-2].x = covers[NUM_COVERS-1].x;

/* move the center cover to the left */
	xpos = ARENA_WIDTH * 0.5f - COVER_W * 1.4f + (CENTER_COVER-1) * COVER_W * 0.125f;

	step_x = CENTER_SPACE / speed;
	step_z = COVER_Z / speed;
	step_angle = COVER_ANGLE / (FPS * 0.35f);		/* turn a bit faster */

	if (covers[CENTER_COVER].x > xpos) {
		covers[CENTER_COVER].x -= step_x;
		if (covers[CENTER_COVER].x < xpos)
			covers[CENTER_COVER].x = xpos;
	}
	if (covers[CENTER_COVER].z > COVER_Z) {
		covers[CENTER_COVER].z += step_z;
		if (covers[CENTER_COVER].z < COVER_Z)
			covers[CENTER_COVER].z = COVER_Z;
	}
	if (covers[CENTER_COVER].angle < COVER_ANGLE) {
		covers[CENTER_COVER].angle += step_angle;
		if (covers[CENTER_COVER].angle > COVER_ANGLE)
			covers[CENTER_COVER].angle = COVER_ANGLE;
	}
	covers[CENTER_COVER].color -= step_color;
	if (covers[CENTER_COVER].color < 0.9f)
		covers[CENTER_COVER].color = 0.9f;

/* move the cover on the right to the center */
	xpos = ARENA_WIDTH * 0.5f;

	if (covers[CENTER_COVER+1].x > xpos) {
		covers[CENTER_COVER+1].x -= step_x;
		if (covers[CENTER_COVER+1].x < xpos)
			covers[CENTER_COVER+1].x = xpos;
	}
	if (covers[CENTER_COVER+1].z < 0.0f) {
		covers[CENTER_COVER+1].z -= step_z;
		if (covers[CENTER_COVER+1].z > 0.0f)
			covers[CENTER_COVER+1].z = 0.0f;
	}
	if (covers[CENTER_COVER+1].angle < 0.0f) {
		covers[CENTER_COVER+1].angle += step_angle;
		if (covers[CENTER_COVER+1].angle > 0.0f)
			covers[CENTER_COVER+1].angle = 0.0f;
	}
	covers[CENTER_COVER+1].color += step_color;
	if (covers[CENTER_COVER+1].color > 1.0f)
		covers[CENTER_COVER+1].color = 1.0f;

/* if centered, stop */
	if (covers[CENTER_COVER+1].x <= xpos && covers[CENTER_COVER+1].angle >= 0.0f) {
		covers[CENTER_COVER+1].color = 1.0f;
		covers[CENTER_COVER].color = 0.9f;
		moving = 0;
		insert_cover_right();
	}
}

void move_cover_right(void) {
	;
}


void draw_cover(Cover *c) {
GLfloat vertex_arr[8] = {
	-COVER_W * 0.5f, COVER_H * 0.5f,
	-COVER_W * 0.5f, -COVER_H * 0.5f,
	COVER_W * 0.5f, COVER_H * 0.5f,
	COVER_W * 0.5f, -COVER_H * 0.5f
};
GLfloat line_arr[8] = {
	-COVER_W * 0.5f, -COVER_H * 0.5f,
	-COVER_W * 0.5f, COVER_H * 0.5f,
	COVER_W * 0.5f, COVER_H * 0.5f,
	COVER_W * 0.5f, -COVER_H * 0.5f
};
GLfloat tex_arr[8] = {
	0, 0,
	0, 1,
	1, 0,
	1, 1,
};
GLfloat tex_reflect[8] = {
	0, 1,
	0, 0,
	1, 1,
	1, 0
};

	glPushMatrix();
	glTranslatef(c->x, c->y, c->z);
	glRotatef(c->angle, 0, 1, 0);

	glColor4f(c->color, c->color, c->color, 1);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, textures[c->texture_idx]);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_arr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

/* draw fake reflection */
	glPushMatrix();
	glTranslatef(0, -COVER_H, 0);
/*
	do not use alpha blending; blend makes the reflections blend thru each other,
	which we don't want to happen
*/
	glColor4f(c->color * 0.25f, c->color * 0.25f, c->color * 0.25f, 1);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_reflect);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glPopMatrix();

/*
	draw border around cover
	for some strange reason, OpenGL rotates the values in the vertex_arr around,
	so we can not reuse it here; I use line_arr instead
*/
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(c->color, c->color, c->color, 1);
	glLineWidth(1.0f);

	glVertexPointer(2, GL_FLOAT, 0, line_arr);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glPopMatrix();
}

void draw_covers(void) {
int i;

	glDisable(GL_DEPTH_TEST);
/*
	draw in this order; without depth buffer
	this is mainly because the covers on the far left and right should be in the back
*/
	for(i = 0; i < CENTER_COVER-1; i++)
		draw_cover(&covers[i]);

	for(i = NUM_COVERS-1; i > CENTER_COVER+1; i--)
		draw_cover(&covers[i]);

/*
	enable depth buffer; otherwise the flipping covers might intersect and look ugly
*/
	glEnable(GL_DEPTH_TEST);

	draw_cover(&covers[CENTER_COVER-1]);
	draw_cover(&covers[CENTER_COVER]);
	draw_cover(&covers[CENTER_COVER+1]);
}

/* EOB */
