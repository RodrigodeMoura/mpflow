/*
	font.h	WJ109
*/

#ifndef FONT_H_WJ109
#define FONT_H_WJ109	1

#define FONT_FILE		"VeraMono.ttf"
#define FONT_SIZE		10

int init_font(void);
void deinit_font(void);
void render_text(int, char *);

#endif	/* FONT_H_WJ109 */

/* EOB */
