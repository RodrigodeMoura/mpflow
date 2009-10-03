/*
	glPrint.c	WJ107

	print bitmapped font to GL screen

	NB. This uses display lists to plot glBitmaps
	    If you'd want to port this code to OpenGLES, it'd be best to
	    plot the text to a pixel buffer, and turn it into a texture
*/

#include "SDK.h"
#include "glPrint.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "font.h"

static GLuint font_list = 0;				/* font display lists */

int font_height = FONT_H;


static void glPrint_string(float x, float y, char *buf) {
char *str, *p;
int len;

/*	glRasterPos2f(x, y);	*/
	glRasterPos2f(0.0f, 0.0f);
	glBitmap(0, 0, 0, 0, x, -y, NULL);

	str = p = buf;
	while(str != NULL) {
		if ((p = strchr(str, '\n')) != NULL) {
			*p = 0;
			len = strlen(str);
			*p = '\n';
			p++;
		} else
			len = strlen(str);

		glCallLists(len, GL_UNSIGNED_BYTE, str);

		if (p != NULL && *p) {
			y += (font_height + LINE_SPACING);		/* plus for upside-down ortho projection */

/*			glRasterPos2f(x, y);	*/
			glRasterPos2f(0.0f, 0.0f);
			glBitmap(0, 0, 0, 0, x, -y, NULL);

			str = p;
		} else
			str = NULL;
	}
}

void glPrint(float x, float y, char *fmt, ...) {
va_list ap;
char buf[GLPRINT_BUF];

	if (fmt == NULL || !*fmt)
		return;

	glPushAttrib(GL_LIST_BIT|GL_ENABLE_BIT);
	glListBase(font_list);
	glDisable(GL_TEXTURE_2D);

	va_start(ap, fmt);
	vsnprintf(buf, GLPRINT_BUF, fmt, ap);
	va_end(ap);

	glPushAttrib(GL_CURRENT_BIT);		/* save current color */
	glColor4f(0, 0, 0, 1);				/* make it black */

	y += (font_height+1);

	glPrint_string(x-1, y-1, buf);
	glPrint_string(x-1, y, buf);
	glPrint_string(x-1, y+1, buf);

	glPrint_string(x+1, y-1, buf);
	glPrint_string(x+1, y, buf);
	glPrint_string(x+1, y+1, buf);

	glPopAttrib();
	glPrint_string(x, y, buf);

	glPopAttrib();
}


/*
	draw a single line with a varied space between the words
*/
static void draw_wide_line(float x, float y, char *str, float space_width) {
char *p;
int len;

/*	glRasterPos2f(x, y);	*/
	glRasterPos2f(0.0f, 0.0f);
	glBitmap(0, 0, 0, 0, x, -y, NULL);

	while(str != NULL) {
		if ((p = strchr(str, ' ')) != NULL) {
			*p = 0;
			len = strlen(str);
			*p = ' ';
			p++;
		} else
			len = strlen(str);

		glCallLists(len, GL_UNSIGNED_BYTE, str);

		if (p != NULL && *p) {
			x += len * FONT_W + space_width;
/*			glRasterPos2f(x, y);	*/
			glRasterPos2f(0.0f, 0.0f);
			glBitmap(0, 0, 0, 0, x, -y, NULL);

			str = p;
		} else
			str = NULL;
	}
}

/*
	print wide lines into a text area with a maximum width
*/
static void glPrint_wide(float max_width, float x, float y, char *buf) {
char *str, *p, *last_space, tmp_char;
int n_chars, n_words, text_width;
float space_width;

	n_chars = n_words = text_width = 0;
	last_space = NULL;

/* split string into words, and fit them in max_width */

	str = p = buf;
	while(*p) {
		if (*p == ' ') {
			last_space = p;
			if (n_chars) {
				n_words++;
				text_width += FONT_W;
				p++;
			}
		} else {
			if (*p == '\n') {
				*p = 0;

				draw_wide_line(x, y, str, FONT_W);
				y += FONT_H + LINE_SPACING;

				*p = '\n';
				p++;
				str = p;
				n_chars = n_words = text_width = 0;
				continue;
			} else {
				n_chars++;
				text_width += FONT_W;
				p++;
			}
		}
		if (text_width >= max_width) {
			if (last_space == NULL) {
				tmp_char = *p;
				*p = 0;

				draw_wide_line(x, y, str, FONT_W);
				y += FONT_H + LINE_SPACING;

				*p = tmp_char;
				str = p;
				n_chars = n_words = text_width = 0;
				continue;
			} else {
				*last_space = 0;

				n_chars = strlen(str);
				space_width = (float)(max_width - (n_chars - (n_words - 1)) * FONT_W) / (float)(n_words - 1);
				if (space_width < FONT_W)
					space_width = FONT_W;

				draw_wide_line(x, y, str, space_width);
				y += FONT_H + LINE_SPACING;

				*last_space = ' ';
				p = last_space;
				p++;
				str = p;
				n_chars = n_words = text_width = 0;
				continue;
			}
		}
	}
	if (*str)				/* draw remaining part */
		draw_wide_line(x, y, str, FONT_W);
}

void glPrintw(float max_width, float x, float y, char *fmt, ...) {
va_list ap;
char buf[GLPRINT_BUF];

	if (fmt == NULL || !*fmt)
		return;

	if (max_width < FONT_W)
		max_width = FONT_W;

	glPushAttrib(GL_LIST_BIT|GL_ENABLE_BIT);
	glListBase(font_list);
	glDisable(GL_TEXTURE_2D);

	va_start(ap, fmt);
	vsnprintf(buf, GLPRINT_BUF, fmt, ap);
	va_end(ap);

	glPushAttrib(GL_CURRENT_BIT);		/* save current color */
	glColor4f(0, 0, 0, 1);				/* make it black */

	y += (font_height+1);

	max_width -= 1.0f;

	glPrint_wide(max_width, x-1, y-1, buf);
	glPrint_wide(max_width, x-1, y, buf);
	glPrint_wide(max_width, x-1, y+1, buf);

	glPrint_wide(max_width, x+1, y-1, buf);
	glPrint_wide(max_width, x+1, y, buf);
	glPrint_wide(max_width, x+1, y+1, buf);

	glPopAttrib();
	glPrint_wide(max_width, x, y, buf);

	glPopAttrib();
}

#ifdef GENERATE_FONT
/*
	load bitmap font file
	The format is 1 byte per scanline per character
	The file may start with a byte specifying the height (which is not used here)
*/
static int load_font(char *filename) {
int fd;

	if ((fd = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "failed to load file '%s'\n", filename);
		return -1;
	}
	if (read(fd, font, 1) < 1) {
		fprintf(stderr, "failed to read file '%s'\n", filename);
		close(fd);
		return -1;
	}
	if (!font[0]) {
		if (read(fd, font+1, 256 * FONT_H - 1) < 256 * FONT_H - 1) {
			fprintf(stderr, "failed to read file '%s'\n", filename);
			close(fd);
			return -1;
		}
	} else {
		if (font[0] > FONT_H) {
			fprintf(stderr, "font height %d > %d, need recompile with larger FONT_H\n", font[0], FONT_H);
			close(fd);
			return -1;
		}
		font_height = font[0];
		if (read(fd, font, 256 * font_height) < 256 * font_height) {
			fprintf(stderr, "failed to read file '%s'\n", filename);
			close(fd);
			return -1;
		}
	}
	close(fd);
	return 0;
}

/*
	glBitmap() is nuts
	it wants the bitmapped font data in an upside-down manner
	You can control the PixelStore in numerous ways but I don't think it does this ...
	so we have to turn it upside-down by ourselves
*/
static void adjust_fontdata(void) {
int i, j;
unsigned char kar[FONT_H];

	for(i = 0; i < 256; i++) {
		memcpy(kar, &font[i * font_height], font_height);

		for(j = 0; j < font_height; j++)
			font[i * font_height + j] = kar[font_height-1 - j];
	}
}

/*
	used during development to dump the font to C-source
*/
static void dump_font(void) {
int i, j, f;

	printf("int font_height = %d;\n\n", font_height);
	printf("static unsigned char font[256 * FONT_H] = {		/* bitmap font */\n");

	f = 0;
	for(j = 0; j < 256; j++) {
		if (j >= ' ' && j < '~')
			printf("/* '%c' */\n\t", j);
		else
			printf("/* char %d */\n\t", j);

		for(i = 0; i < 8; i++)
			printf("0x%02x, ", font[f++]);
		printf("\n\t");

		for(i = 8; i < FONT_H; i++)
			printf("0x%02x, ", font[f++]);
		printf("\n");
	}
	printf("};\n\n\n");
}
#endif	/* GENERATE_FONT */

int init_font(char *font_file) {
int i;

#ifdef GENERATE_FONT
	if (load_font(font_file)) {
		fprintf(stderr, "error: failed to load font '%s'\n", font_file);
		return -1;
	}
	adjust_fontdata();
	dump_font();
#endif
	if (font_list)
		glDeleteLists(font_list, 256);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ((font_list = glGenLists(256)) == 0)
		return -1;

	for(i = 0; i < 256; i++) {
		glNewList(font_list + i, GL_COMPILE);
		glBitmap(8, font_height, 0.0f, 0.0f, 8.0f, 0.0f, &font[i * font_height]);
		glEndList();
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	return 0;
}

int reinit_font(char *font_file) {
int i;

#ifdef GENERATE_FONT
	if (load_font(font_file)) {
		fprintf(stderr, "error: failed to load font '%s'\n", font_file);
		return -1;
	}
	adjust_fontdata();
#endif
	if (font_list)
		glDeleteLists(font_list, 256);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ((font_list = glGenLists(256)) == 0)
		return -1;

	for(i = 0; i < 256; i++) {
		glNewList(font_list + i, GL_COMPILE);
		glBitmap(8, font_height, 0.0f, 0.0f, 8.0f, 0.0f, &font[i * font_height]);
		glEndList();
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	return 0;
}

/*
	return dimensions of text
	This are the dimensions the text would have if rendered with glPrint(),
	not using glPrintw()
*/
void text_dimensions(char *text, int *text_w, int *text_h) {
char *str, *p;
int len, w, h;

	w = h = 0;

	str = p = text;
	while(str != NULL) {
		if ((p = strchr(str, '\n')) != NULL) {
			*p = 0;
			len = strlen(str);
			*p = '\n';
			p++;
		} else
			len = strlen(str);

		if (len > w)
			w = len;

		h++;

		str = p;
	}
	w *= FONT_W;
	h *= (font_height + LINE_SPACING);

	*text_w = w;
	*text_h = h;
}

/* EOB */
