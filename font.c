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

void render_text(int tex_id, char *str) {
SDL_Color color;
SDL_Surface *img;

	delete_texture(tex_id);
	create_texture(tex_id);

	color.r = color.g = color.b = color.unused = 0xff;

	if ((img = TTF_RenderText_Solid(app_font, str, color)) == NULL) {
		fprintf(stderr, "error: render_text() failed to render text\n");
		return;
	}
	surface_to_texture(img, tex_id);
	SDL_FreeSurface(img);
}

/* EOB */
