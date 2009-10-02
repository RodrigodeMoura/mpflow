/*
	texture.h	WJ109
*/

#ifndef TEXTURE_H_WJ109
#define TEXTURE_H_WJ109	1

#include "cover.h"

#define DEFAULT_FOLDER_IMG	"mpflow_folder.png"

#define NUM_COVER_TEXTURES	NUM_COVERS
#define TEX_DEFAULT_FOLDER	NUM_COVERS
#define NUM_TEXTURES		(NUM_COVER_TEXTURES+1)

void init_textures(void);
void deinit_textures(void);
int surface_to_texture(SDL_Surface *, int);
void load_texture(int, char *);
void create_texture(int);
void delete_texture(int);
void bind_texture(int);

#endif	/* TEXTURE_H_WJ109 */

/* EOB */
