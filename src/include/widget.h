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
	void (*mouse_event)(int, int, int, int);
	void (*click_event)(int, int, int);
	void (*drag_event)(int, int);

	Widget *next;
};

extern Widget *main_widget;

int init_widgets(void);
void draw_widgets(void);
void input_widgets(int);
void mouse_widgets(int, int, int, int);
void click_widgets(int, int, int);
void drag_widgets(int, int, int, int);

#endif	/* WIDGET_H_WJ109 */

/* EOB */
