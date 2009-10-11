/*
	widget.h	WJ109
*/

#ifndef WIDGET_H_WJ109
#define WIDGET_H_WJ109	1

typedef struct Widget_tag Widget;

struct Widget_tag {
	int x, y, w, h;

	void (*prepare)(void);
	void (*draw)(void);
	int (*input_event)(int);
	void (*click_event)(int, int, int);

	Widget *next;
};

extern Widget *main_widget;

int init_widgets(void);
void draw_widgets(void);
void input_widgets(int);
void click_widgets(int, int, int);

#endif	/* WIDGET_H_WJ109 */

/* EOB */
