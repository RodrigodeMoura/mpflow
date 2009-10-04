/*
	text.c	WJ108

	text is bitmapped pixel-buffer-text using textures
*/

#include "text.h"
#include "font.h"
#include "power_of_two.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *text_tile;		/* pixel buffer that is used for blitting text */

static Color4 text_colors[8] = {
	{ 0, 0, 0, 0xff },					/* black */
	{ 0xff, 0, 0, 0xff },				/* red */
	{ 0, 0xff, 0, 0xff },				/* green */
	{ 0xff, 0xff, 0, 0xff },			/* yellow */
	{ 0, 0, 0xff, 0xff },				/* blue */
	{ 0xff, 0, 0xff, 0xff },			/* magenta */
	{ 0, 0xff, 0xff, 0xff },			/* cyan */
	{ 0xff, 0xff, 0xff, 0xff },			/* white */
};

static TextBox *textboxes = NULL;
static TextBox static_textbox;			/* single statically allocated textbox (speed optimization) */


void init_text(void) {
/* allocate pixel buffer */
	text_tile = (unsigned char *)malloc(TEXT_TILE_W * TEXT_TILE_H * TEXT_TILE_BPP);
}

#ifdef DYNAMIC_TEXTBOXES
TextBox *new_TextBox(void) {
TextBox *t;

	if ((t = (TextBox *)malloc(sizeof(TextBox))) == NULL)
		return NULL;

	memset(t, 0, sizeof(TextBox));

/* make linked list */
	if (textboxes == NULL)
		textboxes = t;
	else {
		textboxes->prev = t;
		t->next = textboxes;
		textboxes = t;
	}
	return t;
}

void destroy_TextBox(TextBox *t) {
	if (t->next == NULL && t->prev == NULL)
		textboxes = NULL;
	else {
		if (t->prev == NULL) {
			t->next->prev = NULL;
			textboxes = t->next;
		} else {
			t->prev->next = t->next;
			if (t->next != NULL)
				t->next->prev = t->prev;
		}
	}
	if (t->str != NULL)
		free(t->str);

	glDeleteTextures(1, &t->tex_id);			/* hmmm ... what if it was never generated in the first place? */
	free(t);
}
#else	/* no DYNAMIC_TEXTBOXES */
/*
	mpflow needs only 1 textbox active at a time ... so we can optimize greatly
	Ugly? Yes, but it works ;)
*/
TextBox *new_TextBox(void) {
	memset(&static_textbox, 0, sizeof(TextBox));
	textboxes = &static_textbox;
	return &static_textbox;
}

void destroy_TextBox(TextBox *t) {
	if (t->str != NULL)
		free(t->str);

	glDeleteTextures(1, &t->tex_id);

	textboxes = NULL;
}

#endif	/* DYNAMIC_TEXTBOXES */

/*
	scan the complete character from left to right to see what its width is
	it also tells where the first bit is
*/
static void get_bitfont_char_params(int kar, int *bit_start, int *bit_width, int flags) {
int i, j, width, has_bits, start;
unsigned char data;

	*bit_start = 0;
	*bit_width = FONT_W;

	if (kar == ' ' || (flags & TEXT_MONOSPACE))		/* don't bother */
		return;

	width = 0;
	start = -1;
	for(i = 0; i < 8; i++) {
		has_bits = 0;
		for(j = 0; j < font_height; j++) {
			data = font[kar * font_height + j] << i;

			if (start < 0) {
				if (data & 0x80) {
					start = i;
					has_bits = 1;
					break;
				}
			} else
				has_bits |= data;
		}
		if (start >= 0) {
			if (has_bits)
				width++;
			else
				break;
		}
	}
	if (start < 0 || width == 0)	/* it was an empty character (like a space) */
		return;

	*bit_start = start;
	*bit_width = width;
}

/*
	get character width for a single character
*/
static int bitfont_char_width(int kar, int flags) {
int start, width;

	get_bitfont_char_params(kar, &start, &width, flags);
	return width;
}

static int text_width_bitfont(char *str, int flags) {
int w = 0;

	while(*str && *str != '\n') {
		if (*str == ' ')
			w += FONT_W;
		else {
			if (*str >= ' ' && *str < '~')
				w += bitfont_char_width(*str, flags) + FONT_LINING+FONT_SPACING;
		}
		str++;
	}
	return w;
}

/*
	draw text from a bitmap font to the text_tile buffer
	and skipping any leading whitespace in the character bitmap

	x,y is subposition in text_tile
	max_width is the length of the sentence in pixels
	max_width is used so that textures have a minimum size; they are not screen_width wide

	returns the width of the blitted character
*/
static int blit_char(int x, int y, int kar, Color4 *color, int max_width, int flags) {
unsigned char *pixels, data;
int i, j, width, startw;

/*
	to skip any spacing that is present in the bitmapped font, we need to know
	where the first bit is
	and to stop drawing when we're done, we need to know the character width
*/
	get_bitfont_char_params(kar, &startw, &width, flags);

/*
	bitmap data doesn't start till bit #'startw'
*/
	pixels = text_tile + (y * max_width + x) * TEXT_TILE_BPP;

	for(j = 0; j < font_height; j++) {
		data = font[kar * font_height + j] << startw;

		for(i = 0; i < width; i++) {
			if (data & 0x80) {
				pixels[0] = color->r;
				pixels[1] = color->g;
				pixels[2] = color->b;
				pixels[3] = color->a;
			}
			pixels += TEXT_TILE_BPP;
			data <<= 1;
		}
		pixels += (max_width - width) * TEXT_TILE_BPP;
	}
	return width;
}

/*
	blit a line of text into the text_tile buffer
*/
static void blit_text(int x, int y, char *str, Color4 *color, int max_width, int flags) {
	while(*str) {
		switch((unsigned char)*str) {
			case ' ':
				x += FONT_W;
				break;

			case '\r':
				x = 0;
				break;

			case '\n':			/* can only handle a single line */
				return;

			default:
#ifdef TEXTBOX_COLORCODES
/*
	support for changing color within a string using color codes
*/
				if ((unsigned char)*str >= CHAR_BLACK
					&& (unsigned char)*str <= CHAR_WHITE) {				/* change color */
					if (color->a == 0xff && color != &text_colors[BLACK])
						color = &text_colors[(unsigned char)*str - CHAR_BLACK];
				} else
#endif
					x += blit_char(x, y, *str, color, max_width, flags) + FONT_LINING+FONT_SPACING;
		}
		if (x >= max_width)		/* wrap past end of buffer */
			break;

		if (!*str)
			break;

		str++;
	}
}

/*
	blit the shadow mask of the text
*/
static void blit_text_string_mask(char *str, Color4 *color, int max_width, int flags) {
	blit_text(0, 1, str, color, max_width, flags);
	blit_text(2, 1, str, color, max_width, flags);
	blit_text(1, 0, str, color, max_width, flags);
	blit_text(1, 2, str, color, max_width, flags);

	blit_text(0, 0, str, color, max_width, flags);
	blit_text(2, 0, str, color, max_width, flags);
	blit_text(0, 2, str, color, max_width, flags);
	blit_text(2, 2, str, color, max_width, flags);
}

/*
	first blit black outline, then blit text in color
*/
static void blit_text_string(char *str, TextColor color, int max_width, int flags) {
	blit_text_string_mask(str, &text_colors[BLACK], max_width, flags);
	blit_text(1, 1, str, &text_colors[color], max_width, flags);
}

/*
	blit the text and (re)generate texture for the textbox
*/
static void mktexture_textbox(TextBox *box) {
	if (box->str == NULL)
		return;

	glGenTextures(1, &box->tex_id);

/* clear the buffer and blit the text in it */
	memset(text_tile, 0, box->w * box->h * TEXT_TILE_BPP);

	blit_text_string(box->str, box->color, box->w, box->flags);

	glBindTexture(GL_TEXTURE_2D, box->tex_id);

/* apply current lighting to texture */
/*	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);	*/

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

/* text is clamped; do not repeat */
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, box->w, box->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, text_tile);
}

/*
	setup a textbox for rendering text
*/
static TextBox *do_render_text(float x, float y, char *str, TextColor color, int flags) {
TextBox *box;

	if ((box = new_TextBox()) == NULL)
		return NULL;

	if (str != NULL && *str) {
		if ((box->str = strdup(str)) == NULL) {
			destroy_TextBox(box);
			return NULL;
		}
	} else
		box->str = NULL;

	box->x = x;
	box->y = y;

	box->color = color;
	box->flags = flags;

	if (str != NULL && *str)
		box->w = text_width_bitfont(str, box->flags);
	else
		box->w = FONT_W;

	box->h = font_height+FONT_LINING;		/* blit_text() adds black lining */

/* ensure power of two for textures */
	if (!is_power_of_two(box->w))
		box->w = next_power_of_two(box->w);

	if (!is_power_of_two(box->h))
		box->h = next_power_of_two(box->h);

	if (box->w > TEXT_TILE_W)
		box->w = TEXT_TILE_W;

	if (box->h > TEXT_TILE_H)
		box->h = TEXT_TILE_H;

	mktexture_textbox(box);
	return box;
}

/*
	Note: doesn't handle newlines in strings; only good for a single line of text
*/
TextBox *render_text(float x, float y, char *str, TextColor color) {
	return do_render_text(x, y, str, color, 0);
}

/*
	Note: doesn't handle newlines in strings; only good for a single line of text
*/
TextBox *center_text(float x, float y, char *str, TextColor color) {
	if (x < 0)
		x = (screen_width - text_width_bitfont(str, 0)) * 0.5f;

	return do_render_text(x, y, str, color, 0);
}

/*
	Note: doesn't handle newlines in strings; only good for a single line of text
*/
TextBox *render_text_monospace(float x, float y, char *str, TextColor color) {
	return do_render_text(x, y, str, color, TEXT_MONOSPACE);
}

/*
	Note: doesn't handle newlines in strings; only good for a single line of text
*/
TextBox *center_text_monospace(float x, float y, char *str, TextColor color) {
	if (x < 0)
		x = (screen_width - text_width_bitfont(str, TEXT_MONOSPACE)) * 0.5f;

	return render_text_monospace(x, y, str, color);
}

/*
	basically, draw a textured rectangle in orthogonal mode
*/
void draw_textbox(TextBox *box) {
GLfloat vertex_arr[8] = {
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
};
GLfloat tex_arr[8] = {
	0, 0,
	0, 1,
	1, 0,
	1, 1,
};

	if (box->str == NULL)
		return;

	vertex_arr[3] = vertex_arr[7] = box->h;
	vertex_arr[4] = vertex_arr[6] = box->w;

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

#ifdef OPENGLES
	glOrthof(0, screen_width, screen_height, 0, -1, 10);
#else
	glOrtho(0, screen_width, screen_height, 0, -1, 10);
#endif

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(box->x, box->y, 0.0f);

	glColor4f(1, 1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, box->tex_id);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_arr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

/*
	draw fake reflection
	While this seemed like a nice idea, it looks annoying

	glColor4f(0.4f, 0.4f, 0.4f, 1);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_reflect);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
*/
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glDisable(GL_BLEND);
}

void draw_text(void) {
TextBox *box;

	for(box = textboxes; box != NULL; box = box->next)
		draw_textbox(box);
}

/*
	after screen resize, the texture space gets destroyed
	so, regenerate the textures
*/
void reinit_text(void) {
TextBox *box;

	for(box = textboxes; box != NULL; box = box->next) {
		glDeleteTextures(1, &box->tex_id);
		mktexture_textbox(box);
	}
}

/*
	clear all text objects
*/
void clear_text(void) {
TextBox *box, *box_next;

	for(box = textboxes; box != NULL; box = box_next) {
		box_next = box->next;
		destroy_TextBox(box);
	}
}

/* set text position */
void setpos_text(TextBox *box, float x, float y) {
	box->x = x;
	box->y = y;
}

/* EOB */
