/*
	widget.h	WJ109
*/

#ifndef WIDGET_H_WJ109
#define WIDGET_H_WJ109	1

typedef struct Widget_tag Widget;

struct Widget_tag {
	int x, y, w, h;

	void (*draw)(void);
	int (*input_event)(int);
	void (*mouse_event)(int, int, int, int);

	Widget *next;
};

int init_widgets(void);
void draw_widgets(void);
void input_widgets(int);
void mouse_widgets(int, int, int, int);

#endif	/* WIDGET_H_WJ109 */

/* EOB */
