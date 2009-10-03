/*
	glPrint.h	WJ107
*/

#ifndef GL_PRINT_H_WJ107
#define GL_PRINT_H_WJ107	1

#define GLPRINT_BUF			512

#define FONT_W				8			/* 8 bits, one byte */
#define LINE_SPACING		2

extern int font_height;

void glPrint(float, float, char *, ...);
void glPrintw(float, float, float, char *, ...);
int init_font(void);
int reinit_font(char *);
void text_dimensions(char *, int *, int *);

#endif	/* GL_PRINT_H_WJ107 */

/* EOB */
