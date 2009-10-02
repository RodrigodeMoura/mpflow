/*
	texture.c	WJ109
*/

#include "texture.h"
#include "SDK.h"
#include "SDL_image.h"

#include <stdio.h>
#include <stdlib.h>


static GLuint textures[NUM_TEXTURES];
static int inited = 0;


void init_textures(void) {
	if (inited)
		return;

	glGenTextures(NUM_TEXTURES, textures);
	inited = 1;

	load_texture(TEX_DEFAULT_FOLDER, DEFAULT_FOLDER_IMG);
}

void deinit_textures(void) {
	if (inited) {
		glDeleteTextures(NUM_TEXTURES, textures);
		inited = 0;
	}
}

int surface_to_texture(SDL_Surface *img, int tex_id) {
int format;

/* TODO check "power of 2" for img dimensions */

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
	bind_texture(tex_id);
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

void load_texture(int idx, char *filename) {
SDL_Surface *img;

	if ((img = IMG_Load(filename)) == NULL) {
		fprintf(stderr, "error: failed to load image '%s'\n", filename);
		return;
	}
/* make GL texture */
	surface_to_texture(img, idx);

	SDL_FreeSurface(img);
}

void create_texture(int idx) {
	glGenTextures(1, &textures[idx]);
}

void delete_texture(int idx) {
	glDeleteTextures(1, &textures[idx]);
}

void bind_texture(int idx) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[idx]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

/* EOB */
