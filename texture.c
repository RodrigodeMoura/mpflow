/*
	texture.c	WJ109
*/

#include "texture.h"
#include "SDK.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "default_folder.h"

#include <stdio.h>
#include <stdlib.h>

static GLuint textures[NUM_TEXTURES];
static int inited = 0;


#ifdef GENERATE_IMG_DATA
/*
	during development only:
	generate data for the default folder image pixel buffer
*/
void gen_folder_img_data(void) {
SDL_Surface *img;
int i, num_bytes, n_line;

	if ((img = IMG_Load(DEFAULT_FOLDER_IMG)) == NULL) {
		fprintf(stderr, "error: failed to load '%s'\n", DEFAULT_FOLDER_IMG);
		return;
	}
	printf("#define DEFAULT_FOLDER_W\t%d\n", img->w);
	printf("#define DEFAULT_FOLDER_H\t%d\n\n", img->h);
	printf("static unsigned char default_folder_img[] = {\n\t");
	num_bytes = img->w * img->h * 4;

	n_line = 0;
	for(i = 0; i < num_bytes; i++) {
		printf("0x%02x, ", ((unsigned char *)(img->pixels))[i]);

		n_line++;
		if (n_line >= 8) {
			n_line = 0;
			printf("\n\t");
		}
	}
	printf("\n};\n");

	SDL_FreeSurface(img);
}
#endif	/* GENERATE_IMG_DATA */


void init_textures(void) {
	if (inited)
		return;

	glGenTextures(NUM_TEXTURES, textures);
	inited = 1;

#ifdef GENERATE_IMG_DATA
	gen_folder_img_data();
#endif
/*	load_texture(TEX_DEFAULT_FOLDER, DEFAULT_FOLDER_IMG);	*/
	startup_texture();
}

void deinit_textures(void) {
	if (inited) {
		glDeleteTextures(NUM_TEXTURES, textures);
		inited = 0;
	}
}

/*
	make texture from img pixel data
*/
void startup_texture(void) {
	bind_texture(TEX_DEFAULT_FOLDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, s->w, s->h, format, GL_UNSIGNED_BYTE, s->pixels);
*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DEFAULT_FOLDER_W, DEFAULT_FOLDER_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, default_folder_img);

	glDisable(GL_TEXTURE_2D);
}

/*
	quick test to see if x is a power of two
	thanks to wikipedia
*/
int is_power_of_two(int x) {
	if (x <= 0)
		return 0;

	if ((x & (x - 1)) == 0)
		return 1;

	return 0;
}

/*
	get next power of two, without any round(log2) crap
	thanks to wikipedia
*/
int next_power_of_two(int x) {
int i;

	x--;
	for(i = 1; i < sizeof(int) * 8; i <<= 1)
		x = x | (x >> i);
	x++;
	return x;
}

/*
	turn an SDL_Surface into an OpenGL texture
	the trouble with this is, that OpenGL likes powers of two
	(actually, my NVIDIA card likes all kinds of dimensions, but other brands of video chips don't)
	so use rotozoom to scale to a power of two -- rotozoom works very well, I say
*/
int surface_to_texture(SDL_Surface *img, int tex_id) {
SDL_Surface *s;
int format;

	if (img->w <= 0 || img->h <= 0)
		return -1;

	if (!(is_power_of_two(img->w) && is_power_of_two(img->h))) {
		int w, h;
		double zoom_x, zoom_y;

		w = next_power_of_two(img->w);
		h = next_power_of_two(img->h);

		zoom_x = (double)w / (double)img->w;
		zoom_y = (double)h / (double)img->h;

		if ((s = zoomSurface(img, zoom_x, zoom_y, SMOOTHING_ON)) == NULL) {
			fprintf(stderr, "error: zoomSurface() failed\n");
			return -1;
		}
	} else
		s = img;

	switch(s->format->BytesPerPixel) {
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

			if (s != img)
				SDL_FreeSurface(s);

			return -1;
	}
	bind_texture(tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, s->w, s->h, format, GL_UNSIGNED_BYTE, s->pixels);
*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels);

	glDisable(GL_TEXTURE_2D);

	if (s != img)
		SDL_FreeSurface(s);

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

void create_textures(int range) {
	glGenTextures(range, textures);
}

void delete_texture(int idx) {
	glDeleteTextures(1, &textures[idx]);
}

void delete_textures(int range) {
	glDeleteTextures(range, textures);
}

void bind_texture(int idx) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[idx]);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

/* EOB */
