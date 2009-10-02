/*
	font.h	WJ109
*/

#ifndef FONT_H_WJ109
#define FONT_H_WJ109	1

#define FONT_FILE		"VeraMoBd.ttf"
#define FONT_SIZE		16

int init_font(void);
void deinit_font(void);
void render_text(int, char *, int *, int *);

#endif	/* FONT_H_WJ109 */

/* EOB */
