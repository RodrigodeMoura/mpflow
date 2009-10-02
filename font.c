/*
	font.c	WJ109
*/

#include "font.h"
#include "SDL_ttf.h"
#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>


static TTF_Font *app_font = NULL;


int init_font(void) {
struct stat statbuf;

	if (TTF_Init() == -1) {
		fprintf(stderr, "error: failed to initialize font: %s\n", TTF_GetError());
		return -1;
	}
	if (stat(FONT_FILE, &statbuf) == -1) {
		fprintf(stderr, "error: font file %s not found\n", FONT_FILE);
		return -1;
	}
	if ((app_font = TTF_OpenFont(FONT_FILE, FONT_SIZE)) == NULL) {
		fprintf(stderr, "error: failed to load font '%s': %s\n", FONT_FILE, TTF_GetError());
		return -1;
	}
	return 0;
}

void deinit_font(void) {
	if (app_font != NULL) {
		TTF_CloseFont(app_font);
		app_font = NULL;
	}
	TTF_Quit();
}

int power_of_two(int n) {
double log2;
int log2i;

	log2 = log(n) / log(2);
	log2i = (int)ceil(log2);
	return (int)(pow(2,log2i));
}

void render_text(int tex_id, char *str, int *w, int *h) {
SDL_Color color;
SDL_Surface *img, *tmp;
SDL_Rect r;
int width, height, text_w, text_h;

	color.r = color.g = color.b = 0xff;
	color.unused = 0xff;

	if ((img = TTF_RenderText_Solid(app_font, str, color)) == NULL) {
		fprintf(stderr, "error: render_text() failed to render text\n");
		return;
	}
	text_w = img->w;
	text_h = img->h;

	*w = width = power_of_two(img->w);
	*h = height = power_of_two(img->h);

	if ((tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000)) == NULL) {
		SDL_FreeSurface(img);
		fprintf(stderr, "error: render_text() failed to render text\n");
		return;
	}
	r.x = (width - text_w) / 2;
	r.y = 0; /* (height - text_h) / 2; */

	SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 0));
	SDL_BlitSurface(img, NULL, tmp, &r);
	SDL_FreeSurface(img);

	surface_to_texture(tmp, tex_id);

	SDL_FreeSurface(tmp);
}

/* EOB */
