/*
	widget_about.c	WJ109
*/

#include "widget_about.h"
#include "widget_aboutbutton.h"
#include "widget_titlebar.h"
#include "widget_covers.h"
#include "main.h"
#include "cover.h"
#include "font.h"
#include "text.h"
#include "texture.h"
#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>

Widget w_about;

static void prepare_about(void);
static void draw_about(void);
static int input_about(int);
static void click_about(int, int, int);

void init_widget_about(void) {
	memset(&w_about, 0, sizeof(Widget));

	w_about.x = w_about.y = 0;
	w_about.w = screen_width;
	w_about.h = screen_height;

	w_about.prepare = prepare_about;
	w_about.draw = draw_about;
	w_about.input_event = input_about;
	w_about.click_event = click_about;

	w_about.next = NULL;
}

static void prepare_about(void) {
	clear_text();
	center_text(-1, screen_height - FONT_H * 2, "mpflow Copyright (C) 2009 Walter de Jong <walter@heiho.net>", YELLOW);
}

static void draw_about(void) {
Cover c;

	c.x = ARENA_WIDTH * 0.5f;
	c.y = ARENA_HEIGHT * 0.5f;
	c.z = 0;
	c.angle = 0;
	c.color = 1;
	c.pos = CENTER_COVER;
	c.texture_idx = TEX_DEFAULT_FOLDER;
	c.dirlist = NULL;

	draw_cover(&c);
	draw_text();
}

static int input_about(int key) {
	switch(key) {
		case SDK_ESC:
			exit_program(0);
			return 1;

		case SDK_ENTER:
		case SDK_SPACE:
			main_widget = &w_covers;
			main_widget->prepare();
			draw();
			return 1;

		default:
			;
	}
	return 0;
}

static void click_about(int button, int x, int y) {
	if (button == SDK_MOUSE_LEFT || button == SDK_MOUSE_RIGHT) {
		w_titlebar.next = &w_about_button1;
		prepare_widgets();
		draw();
	}
}

/* EOB */
