/*
	glPrint.h	WJ107
*/

#ifndef GL_PRINT_H_WJ107
#define GL_PRINT_H_WJ107	1

#define GLPRINT_BUF			512

void glPrint(float, float, char *, ...);
void glPrintw(float, float, float, char *, ...);
int init_font(void);
int reinit_font(char *);
void text_dimensions(char *, int *, int *);

#endif	/* GL_PRINT_H_WJ107 */

/* EOB */
