/*
	mpflow	WJ109
*/

#include "main.h"
#include "SDK.h"
#include "SDL_image.h"

#include <stdio.h>
#include <stdlib.h>


int screen_width = 600;					/* current display resolution (or window size) */
int screen_height = 200;

int max_xres = 320, max_yres = 200;		/* maximum display resolution (as reported by SDK_max_videomode()) */

int key_down = 0;

GLuint textures[10];


void draw(void);


void init_gl(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

#ifdef OPENGLES
	glOrthof(0.0f, ARENA_WIDTH, 0.0f, ARENA_HEIGHT, -Z_FAR, Z_FAR);
#else
	glOrtho(0.0f, ARENA_WIDTH, 0.0f, ARENA_HEIGHT, -Z_FAR, Z_FAR);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);			/* GL_TRUE */

/*	glEnable(GL_LIGHTING);	*/

	glEnable(GL_TEXTURE_2D);

/* alpha channel transparency */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

/* not needed
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
*/
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

/* simply always turn this on */
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
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
	screen_height = screen_width * 0.2f;

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

void draw_cover(void) {
GLfloat vertex_arr[8];
GLfloat tex_arr[8] = {
	0, 0,
	0, 1,
	1, 0,
	1, 1,
};

	glColor4f(1, 1, 1, 1);

	vertex_arr[0] = vertex_arr[2] = 0;
	vertex_arr[1] = vertex_arr[5] = COVER_W;
	vertex_arr[3] = vertex_arr[7] = 0;
	vertex_arr[4] = vertex_arr[6] = COVER_H;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0]);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_arr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef((ARENA_WIDTH - COVER_W) * 0.5f, (ARENA_HEIGHT - COVER_H) * 0.5f, 0.0f);
	draw_cover();

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
