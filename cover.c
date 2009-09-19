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


void draw_cover(float color) {
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

	glColor4f(color, color, color, 1);

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

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
	glColor4f(color * 0.25f, color * 0.25f, color * 0.25f, 1);

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

	glColor4f(color, color, color, 1);
	glLineWidth(1.0f);

	glVertexPointer(2, GL_FLOAT, 0, line_arr);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void draw_covers(void) {
float center_x, center_y, pos, color;
int n;

	center_x = ARENA_WIDTH * 0.5f;
	center_y = ARENA_HEIGHT * 0.5f;

/* left side */
	color = 0.5f;
	pos = center_x - COVER_W * 1.4f;
	for(n = 0; n < 5; n++) {
		glPushMatrix();
		glTranslatef(pos, center_y, -50);
		glRotatef(60.0f, 0, 1, 0);
		draw_cover(color);
		glPopMatrix();

		pos += (COVER_W * 0.125f);
		color += 0.1f;
	}

/* right side */
	color = 0.5f;
	pos = center_x + COVER_W * 1.4f;
	for(n = 0; n < 5; n++) {
		glPushMatrix();
		glTranslatef(pos, center_y, -50);
		glRotatef(-60.0f, 0, 1, 0);
		draw_cover(color);
		glPopMatrix();

		pos -= (COVER_W * 0.125f);
		color += 0.1f;
	}

/* center cover */
	glPushMatrix();
	glTranslatef(center_x, center_y, 0);
	draw_cover(1);
	glPopMatrix();
}

/* EOB */
