/*
	texture.c	WJ109
*/

#include "texture.h"
#include "SDL_image.h"

#include <stdio.h>
#include <stdlib.h>


static GLuint textures[NUM_TEXTURES];


void init_textures(void) {
	glGenTextures(NUM_TEXTURES, textures);

	load_texture(TEX_DEFAULT_FOLDER, DEFAULT_FOLDER_JPG);
}

static int load_texture_img(SDL_Surface *img, GLuint tex_id) {
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

void load_texture(int idx, char *filename) {
SDL_Surface *img;

	if ((img = IMG_Load(filename)) == NULL) {
		fprintf(stderr, "error: failed to load image '%s'\n", filename);
		return;
	}
/* make GL texture */

	load_texture_img(img, textures[idx]);

	SDL_FreeSurface(img);
}

void create_texture(int idx) {
	glGenTextures(1, &textures[idx]);
}

void delete_texture(int idx) {
	glDeleteTextures(1, &textures[idx]);
}

void bind_texture(int idx) {
	glBindTexture(GL_TEXTURE_2D, textures[idx]);
}

/* EOB */
