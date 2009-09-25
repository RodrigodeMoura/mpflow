/*
	texture.h	WJ109
*/

#ifndef TEXTURE_H_WJ109
#define TEXTURE_H_WJ109	1

#include "SDK.h"

#define DEFAULT_FOLDER_JPG	"mpflow_folder.jpg"

#define NUM_TEXTURES		14
#define TEX_DEFAULT_FOLDER	(NUM_TEXTURES-1)

void init_textures(void);
void load_texture(int, char *);
void create_texture(int);
void delete_texture(int);
void bind_texture(int);

#endif	/* TEXTURE_H_WJ109 */

/* EOB */
