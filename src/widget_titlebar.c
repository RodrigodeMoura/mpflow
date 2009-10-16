/*
	widget_titlebar.c	WJ109
*/

#include "widget_titlebar.h"
#include "widget_aboutbutton.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Widget w_titlebar;

static void drag_titlebar(int, int);

void init_widget_titlebar(void) {
	memset(&w_titlebar, 0, sizeof(Widget));

	w_titlebar.x = screen_width / 10;
	w_titlebar.y = 0;
	w_titlebar.w = screen_width - 2 * w_titlebar.x;
	w_titlebar.h = screen_height / 8;

	w_titlebar.drag_event = drag_titlebar;

	w_titlebar.next = &w_about_button1;
}

/*
 	drag main window
*/
static void drag_titlebar(int dx, int dy) {
	move_app_window(window_x + dx, window_y + dy);
}

/* EOB */
