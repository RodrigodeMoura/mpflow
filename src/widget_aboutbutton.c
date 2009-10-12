/*
	widget_aboutbutton.c	WJ109
*/

#include "widget_aboutbutton.h"
#include "widget_about.h"
#include "widget_titlebar.h"
#include "widget_covers.h"
#include "SDK.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Widget w_about_button1, w_about_button2;


static void click_button(int, int, int);

void init_widget_aboutbutton(void) {
	memset(&w_about_button1, 0, sizeof(Widget));
	memset(&w_about_button2, 0, sizeof(Widget));

	w_about_button1.x = w_about_button1.y = 0;
	w_about_button1.w = screen_width / 10;
	w_about_button1.h = screen_height / 10;

	w_about_button1.click_event = click_button;

	w_about_button1.next = &w_about_button2;

	w_about_button2.x = screen_width - w_about_button1.w;
	w_about_button2.y = 0;
	w_about_button2.w = w_about_button1.w;
	w_about_button2.h = w_about_button1.h;

	w_about_button2.click_event = click_button;

	w_about_button2.next = &w_covers;
}

static void click_button(int button, int x, int y) {
	if (button & (SDK_MOUSE_LEFT|SDK_MOUSE_RIGHT)) {
		w_titlebar.next = &w_about;
		prepare_widgets();
		draw();
	}
}

/* EOB */
