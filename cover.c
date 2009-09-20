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
float xpos, ypos, zpos, color;

/* left side */
	xpos = ARENA_WIDTH * 0.5f - COVER_W * 1.4f;
	ypos = ARENA_HEIGHT * 0.5f;
	zpos = -50;
	color = 0.5f;

	for(i = 0; i < CENTER_COVER; i++) {
		covers[i].x = xpos;
		covers[i].y = ypos;
		covers[i].z = zpos;
		covers[i].angle = 60;
		covers[i].color = color;
		covers[i].texture_idx = 0;

		xpos += (COVER_W * 0.125f);
		color += 0.1f;
	}

/* right side */
	xpos = ARENA_WIDTH * 0.5f + COVER_W * 1.4f;
	color = 0.5f;

	for(i = NUM_COVERS-1; i > CENTER_COVER; i--) {
		covers[i].x = xpos;
		covers[i].y = ypos;
		covers[i].z = zpos;
		covers[i].angle = -60;
		covers[i].color = color;
		covers[i].texture_idx = 0;

		xpos -= (COVER_W * 0.125f);
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
int i, tex;

/* TODO: delete texture that was on the right */

	tex = covers[NUM_COVERS-1].texture_idx;

	for(i = NUM_COVERS-1; i > 0; i--) {
		covers[i].x = covers[i-1].x;
		covers[i].y = covers[i-1].y;
		covers[i].z = covers[i-1].z;
		covers[i].angle = covers[i-1].angle;
		covers[i].color = covers[i-1].color;
		covers[i].texture_idx = covers[i-1].texture_idx;
	}
	covers[0].texture_idx = tex;
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
	covers[NUM_COVERS-1].texture_idx = tex;
}

void move_cover_left(void) {
float xpos, zpos;
int i;

	for(i = CENTER_COVER+1; i < NUM_COVERS; i++) {
		covers[i].x -= 0.5f;
	}
/* move the center cover to the left */
	xpos = ARENA_WIDTH * 0.5f - COVER_W * 1.4f + (CENTER_COVER-1) * COVER_W * 0.125f;
	zpos = -50;

	if (covers[CENTER_COVER].x > xpos) {
		covers[CENTER_COVER].x -= 1;
		if (covers[CENTER_COVER].x < xpos)
			covers[CENTER_COVER].x = xpos;
	}
	if (covers[CENTER_COVER].z > zpos) {
		covers[CENTER_COVER].z -= 1;
		if (covers[CENTER_COVER].z < zpos)
			covers[CENTER_COVER].z = zpos;
	}
	if (covers[CENTER_COVER].angle < 60) {
		covers[CENTER_COVER].angle += 1;
		if (covers[CENTER_COVER].angle > 60)
			covers[CENTER_COVER].angle = 60;
	}
/* move the cover on the right to the center */
	xpos = ARENA_WIDTH * 0.5f;
	zpos = 0;

	if (covers[CENTER_COVER+1].x > xpos) {
		covers[CENTER_COVER+1].x -= 1;
		if (covers[CENTER_COVER+1].x < xpos)
			covers[CENTER_COVER+1].x = xpos;
	}
	if (covers[CENTER_COVER+1].z < zpos) {
		covers[CENTER_COVER+1].z += 1;
		if (covers[CENTER_COVER+1].z > zpos)
			covers[CENTER_COVER+1].z = zpos;
	}
	if (covers[CENTER_COVER+1].angle < 0) {
		covers[CENTER_COVER+1].angle += 1;
		if (covers[CENTER_COVER+1].angle > 0)
			covers[CENTER_COVER+1].angle = 0;
	}
/* if centered, stop */
	if (covers[CENTER_COVER+1].x <= xpos && covers[CENTER_COVER+1].angle >= 0) {
		moving = 0;
		insert_cover_right();
	}
}

void move_cover_right(void) {
float xpos, zpos;
int i;

	for(i = 0; i < CENTER_COVER; i++) {
		covers[i].x += 0.5f;
	}
/* move center cover to the right */
	xpos = ARENA_WIDTH * 0.5f + COVER_W * 1.4f - (CENTER_COVER-1) * COVER_W * 0.125f;
	zpos = -50;

	if (covers[CENTER_COVER].x < xpos) {
		covers[CENTER_COVER].x += 1;
		if (covers[CENTER_COVER].x > xpos)
			covers[CENTER_COVER].x = xpos;
	}
	if (covers[CENTER_COVER].z > zpos) {
		covers[CENTER_COVER].z -= 1;
		if (covers[CENTER_COVER].z < zpos)
			covers[CENTER_COVER].z = zpos;
	}
	if (covers[CENTER_COVER].angle > -60) {
		covers[CENTER_COVER].angle -= 1;
		if (covers[CENTER_COVER].angle < -60)
			covers[CENTER_COVER].angle = -60;
	}
/* move the cover on the left to the center */
	xpos = ARENA_WIDTH * 0.5f;
	zpos = 0;

	if (covers[CENTER_COVER-1].x < xpos) {
		covers[CENTER_COVER-1].x += 1;
		if (covers[CENTER_COVER-1].x > xpos)
			covers[CENTER_COVER-1].x = xpos;
	}
	if (covers[CENTER_COVER-1].z < zpos) {
		covers[CENTER_COVER-1].z += 1;
		if (covers[CENTER_COVER-1].z > zpos)
			covers[CENTER_COVER-1].z = zpos;
	}
	if (covers[CENTER_COVER-1].angle > 0) {
		covers[CENTER_COVER-1].angle -= 1;
		if (covers[CENTER_COVER-1].angle < 0)
			covers[CENTER_COVER-1].angle = 0;
	}
/* if centered, stop */
	if (covers[CENTER_COVER-1].x >= xpos && covers[CENTER_COVER-1].angle >= 0) {
		moving = 0;
		insert_cover_left();
	}
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

	for(i = 0; i < NUM_COVERS; i++)
		draw_cover(&covers[i]);
}

/* EOB */
