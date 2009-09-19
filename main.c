/*
	mpflow	WJ109
*/

#include "main.h"
#include "SDK.h"
#include "SDL_image.h"
#include "glut.h"

#include <stdio.h>
#include <stdlib.h>


int screen_width = 600;					/* current display resolution (or window size) */
int screen_height = 200;

int max_xres = 320, max_yres = 200;		/* maximum display resolution (as reported by SDK_max_videomode()) */

int key_down = 0;

GLuint textures[11];


void draw(void);


void init_gl(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)ARENA_WIDTH / (float)ARENA_HEIGHT, 0.01f, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);			/* GL_TRUE */

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

/* simply always turn this on */
	glEnableClientState(GL_VERTEX_ARRAY);
}

void handle_keypress(int key) {
	switch(key) {
		case SDK_ESC:
			SDK_exit(0);
			break;

		default:
			;
	}
}

void key_event(SDK_Event state, int key) {
	if (state == SDK_PRESS) {
		key_down = key;
		handle_keypress(key);
	} else
		key_down = 0;
}

void window_event(SDK_Event event, int w, int h) {
	switch(event) {
		case SDK_CLOSE:
			SDK_exit(0);
			break;

		case SDK_EXPOSE:
			draw();
			break;

		default:
			;
	}
}

void create_window(void) {
	SDK_max_videomode(&max_xres, &max_yres);

	screen_width = max_xres * 0.6f;
	screen_height = screen_width * 0.4f;

	SDK_window_hints(SDL_NOFRAME);

	if (SDK_create_window(screen_width, screen_height, 0, "mpflow") < 0) {
		fprintf(stderr, "error: failed to create window\n");
		SDK_exit(1);
	}
}

int create_texture(SDL_Surface *img, GLuint tex_id) {
int format;

	switch(img->format->BytesPerPixel) {
		case 1:								/* grayscale jpg? */
			format = GL_COLOR_INDEX;		/* so, this is probaby wrong ... */
			break;

		case 3:
			format = GL_RGB;
			break;

		case 4:
			format = GL_RGBA;
			break;

		default:
			format = -1;		/* invalid */
			fprintf(stderr, "create_texture(): invalid pixel format\n");
			return -1;
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img->w, img->h, format, GL_UNSIGNED_BYTE, img->pixels);
*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, img->pixels);

	glDisable(GL_TEXTURE_2D);
	return 0;
}

void load(void) {
SDL_Surface *img;

	if ((img = IMG_Load("/media/idisk/mp3/Muse/The Resistance/cover.jpg")) == NULL) {
		fprintf(stderr, "error: failed to load image\n");
		return;
	}
/* make GL texture */

	glGenTextures(1, textures);
	create_texture(img, textures[0]);

	SDL_FreeSurface(img);
}

/*
	draw white frame around window
*/
void draw_window_border(void) {
GLfloat vertex_arr[8] = {
	0, 0,
	ARENA_WIDTH, 0,
	ARENA_WIDTH, ARENA_HEIGHT,
	0, ARENA_HEIGHT
};

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
#ifdef OPENGLES
	glOrthof(0.0f, ARENA_WIDTH, 0.0f, ARENA_HEIGHT, -Z_FAR, Z_FAR);
#else
	glOrtho(0.0f, ARENA_WIDTH, 0.0f, ARENA_HEIGHT, -Z_FAR, Z_FAR);
#endif
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1, 1, 1, 1);
	glLineWidth(4.0f);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void draw_cover(void) {
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

	glColor4f(1, 1, 1, 1);

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
	glColor4f(0.25f, 0.25f, 0.25f, 1);

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

	glColor4f(1, 1, 1, 1);
	glLineWidth(1.0f);

	glVertexPointer(2, GL_FLOAT, 0, line_arr);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void draw(void) {
float center_x, center_y, pos;
int n;

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(-ARENA_WIDTH * 0.5f, -ARENA_HEIGHT * 0.5f, -180);

	center_x = ARENA_WIDTH * 0.5f;
	center_y = ARENA_HEIGHT * 0.5f;

/* left side */
	pos = center_x - COVER_W * 1.4f;
	for(n = 0; n < 5; n++) {
		glPushMatrix();
		glTranslatef(pos, center_y, -50);
		glRotatef(60.0f, 0, 1, 0);
		draw_cover();
		glPopMatrix();

		pos += (COVER_W * 0.125f);
	}

/* right side */
	pos = center_x + COVER_W * 1.4f;
	for(n = 0; n < 5; n++) {
		glPushMatrix();
		glTranslatef(pos, center_y, -50);
		glRotatef(-60.0f, 0, 1, 0);
		draw_cover();
		glPopMatrix();

		pos -= (COVER_W * 0.125f);
	}

/* center cover */
	glPushMatrix();
	glTranslatef(center_x, center_y, 0);
	draw_cover();
	glPopMatrix();

	draw_window_border();

	glFlush();
	SDK_swapbuffers();
}


int main(int argc, char *argv[]) {
	SDK_init();

	create_window();

	init_gl();
	load();

	SDK_key_event(key_event);
	SDK_window_event(window_event);

	draw();

	for(;;) {
		SDK_handle_events();
		SDK_sleep(200);
	}
	return 0;
}

/* EOB */
