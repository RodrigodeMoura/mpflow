/*
	widget_about.c	WJ109
*/

#include "widget_about.h"
#include "widget_covers.h"
#include "main.h"
#include "cover.h"
#include "text.h"
#include "texture.h"
#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>

Widget w_about;

static void draw_about(void);
static int input_about(int);

void init_widget_about(void) {
	w_about.x = w_about.y = 0;
	w_about.w = screen_width;
	w_about.h = screen_height;

	w_about.draw = draw_about;
	w_about.input_event = input_about;
	w_about.mouse_event = NULL;

	w_about.next = NULL;
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
			return 1;

		default:
			;
	}
	return 0;
}

/* EOB */
