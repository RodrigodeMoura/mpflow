/*
	widget_titlebar.c	WJ109
*/

#include "widget_titlebar.h"
#include "widget_aboutbutton.h"
#include "main.h"
#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Widget w_titlebar;

static int dir_x, dir_y, dir_x_change, dir_y_change;
static unsigned int last_drag;

static void mouse_titlebar(int, int, int, int);
static void drag_titlebar(int, int);

void init_widget_titlebar(void) {
	memset(&w_titlebar, 0, sizeof(Widget));

	w_titlebar.x = screen_width / 10;
	w_titlebar.y = 0;
	w_titlebar.w = screen_width - 2 * w_titlebar.x;
	w_titlebar.h = screen_height / 8;

	w_titlebar.mouse_event = mouse_titlebar;
	w_titlebar.drag_event = drag_titlebar;

	w_titlebar.next = &w_about_button1;

	dir_x = dir_y = dir_x_change = dir_y_change = 0;
	last_drag = 0;
}

/*
	stopped shaking the window; play random
*/
static void mouse_titlebar(int event, int buttons, int x, int y) {
	if (event == SDK_RELEASE) {
		if (buttons & SDK_MOUSE_LEFT) {
			if (dir_x_change + dir_y_change >= WINDOW_SHAKE)
				play_random();

			dir_x = dir_y = dir_x_change = dir_y_change = 0;
			last_drag = 0;
		}
	}
}

/*
	drag main window
*/
static void drag_titlebar(int dx, int dy) {
unsigned int t, dt;

	move_app_window(window_x + dx, window_y + dy);

/* keep stats about directional changes for detecting 'window shake' */
	t = SDK_ticks();
	dt = t - last_drag;
	last_drag = t;

	if (dt > SHAKE_INTERVAL) {
		dir_x = dir_y = dir_x_change = dir_y_change = 0;
		return;
	}
	if (dx < 0) {
		if (dir_x >= 0)
			dir_x_change++;
		dir_x = -1;
	} else {
		if (dx > 0) {
			if (dir_x <= 0)
				dir_x_change++;
			dir_x = 1;
		}
	}
	if (dy < 0) {
		if (dir_y >= 0)
			dir_y_change++;
		dir_y = -1;
	} else {
		if (dy > 0) {
			if (dir_y <= 0)
				dir_y_change++;
			dir_y = 1;
		}
	}
}

/* EOB */
