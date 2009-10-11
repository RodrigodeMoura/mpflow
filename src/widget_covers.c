/*
	widget_covers.c	WJ109
*/

#include "widget_covers.h"
#include "cover.h"
#include "mpd.h"
#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>

Widget w_covers;

static int input_covers(int);


void init_widget_covers(void) {
	w_covers.x = w_covers.y = 0;
	w_covers.w = screen_width;
	w_covers.h = screen_height;

	w_covers.draw = draw_covers;
	w_covers.input_event = input_covers;
	w_covers.mouse_event = NULL;

	w_covers.next = NULL;

	main_widget = &w_covers;
}

static int input_covers(int key) {
	switch(key) {
		case SDK_ESC:
			exit_program(0);
			return 1;

		case SDK_ENTER:
			play_album(covers[CENTER_COVER].dirlist->path);
			return 1;

		case SDK_SPACE:
			play_pause();
			return 1;

		case SDK_BACKSPACE:
			play_next();
			return 1;

		case SDK_TAB:
			play_random();
			return 1;

		default:
			jump_to_cover(key);
			return 1;
	}
	return 0;		/* unrecognized key, pass it on */
}

/* EOB */
