/*
	widget.c	WJ109
*/

#include "widget.h"

#include <stdio.h>
#include <stdlib.h>

Widget *main_widget = NULL;


int init_widgets(void) {
	;
}

void prepare_widgets(void) {
Widget *w;

	for(w = main_widget; w != NULL; w = w->next) {
		if (w->prepare != NULL)
			w->prepare();
	}
}

void draw_widgets(void) {
Widget *w;

	for(w = main_widget; w != NULL; w = w->next) {
		if (w->draw != NULL)
			w->draw();
	}
}

void input_widgets(int key) {
Widget *w;

	for(w = main_widget; w != NULL; w = w->next) {
		if (w->input_event != NULL)
			if (w->input_event(key))
				break;
	}
}

void mouse_widgets(int event, int buttons, int x, int y) {
Widget *w;

	for(w = main_widget; w != NULL; w = w->next) {
		if (w->mouse_event != NULL
		    && x >= w->x && x <= w->x + w->w
		    && y >= w->y && y <= w->y + w->h)
		 	w->mouse_event(event, buttons, x, y);
	}
}

void click_widgets(int button, int x, int y) {
Widget *w;

	for(w = main_widget; w != NULL; w = w->next) {
		if (w->click_event != NULL
		    && x >= w->x && x <= w->x + w->w
		    && y >= w->y && y <= w->y + w->h)
		 	w->click_event(button, x, y);
	}
}

void drag_widgets(int wx, int wy, int dx, int dy) {
Widget *w;

	for(w = main_widget; w != NULL; w = w->next) {
		if (w->drag_event != NULL
		    && wx >= w->x && wx <= w->x + w->w
		    && wy >= w->y && wy <= w->y + w->h)
		 	w->drag_event(dx, dy);
	}
}

/* EOB */
