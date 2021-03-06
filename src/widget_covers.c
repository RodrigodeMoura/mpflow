/*
	widget_covers.c	WJ109
*/

#include "widget_covers.h"
#include "cover.h"
#include "mpd.h"
#include "SDK.h"
#include "event.h"
#include "text.h"
#include "texture.h"

#include <stdio.h>
#include <stdlib.h>

Widget w_covers;

static unsigned int center_clicked;
static SDL_Rect left_side, right_side;
static int scroll_wheel;

int moving;
unsigned int ticks_moving;


static void prepare_covers(void);
static int input_covers(int);
static void mouse_covers(int, int, int, int);
static void click_covers(int, int, int);


void init_widget_covers(void) {
	memset(&w_covers, 0, sizeof(Widget));

	w_covers.x = w_covers.y = 0;
	w_covers.w = screen_width;
	w_covers.h = screen_height;

	w_covers.prepare = prepare_covers;
	w_covers.draw = draw_covers;
	w_covers.input_event = input_covers;
	w_covers.mouse_event = mouse_covers;
	w_covers.click_event = click_covers;

	w_covers.next = NULL;
/*
	setup areas for mouse clicks
	NB. init_covers() must have been called before this function
*/
	left_side.x = 0;
	left_side.y = center_cover.y;
	left_side.w = center_cover.x;
	left_side.h = center_cover.h;

	right_side.x = center_cover.x + center_cover.w;
	right_side.y = center_cover.y;
	right_side.w = screen_width - right_side.x;
	right_side.h = center_cover.h;

	center_clicked = 0;

	moving = 0;
	ticks_moving = 0;
	scroll_wheel = 0;
}

static void prepare_covers(void) {
	clear_text();
	reset_cover_title_text();
}

static void jump_to_cover(int key) {
	if (jump_cover(key) == -1)
		return;

	set_cover_dirlist();
	delete_textures(NUM_COVER_TEXTURES);
	create_textures(NUM_COVER_TEXTURES);
	load_cover_textures();

	draw();
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

static void mouse_covers(int event, int buttons, int x, int y) {
	if (event == SDK_PRESS) {
/* use mouse wheel to scroll */
		if (buttons & SDK_MOUSE_WHEELUP) {
			if (!moving) {
				moving = MOVE_LEFT;
				ticks_moving = SDK_ticks();
			}
			if (moving == MOVE_RIGHT)
				scroll_wheel = 0;
			else
				scroll_wheel++;
		}
		if (buttons & SDK_MOUSE_WHEELDOWN) {
			if (!moving) {
				moving = MOVE_RIGHT;
				ticks_moving = SDK_ticks();
			}
			if (moving == MOVE_LEFT)
				scroll_wheel = 0;
			else
				scroll_wheel++;
		}
	}
}

static void click_covers(int button, int x, int y) {
	if (button & SDK_MOUSE_LEFT) {
/* double click on center cover plays the album */
		if (click_rect(&center_cover, x, y)) {
			if (SDK_ticks() - center_clicked <= MOUSE_DOUBLECLICK)
				play_album(covers[CENTER_COVER].dirlist->path);
			else
/* single click pauses */
				play_pause();

			center_clicked = SDK_ticks();
		} else {
			if (click_rect(&left_side, x, y)) {
				moving = MOVE_RIGHT;
				ticks_moving = SDK_ticks();
			} else {
				if (click_rect(&right_side, x, y)) {
					moving = MOVE_LEFT;
					ticks_moving = SDK_ticks();
				}
			}
		}
	}

/* right click: skip; play next song */
	if (button & SDK_MOUSE_RIGHT) {
		if (click_rect(&center_cover, x, y))
			play_next();
	}
}

void move_covers(void) {
int buttons, x, y, was_moving;

	was_moving = moving;

	if (!moving) {
		ticks_moving = 0;

		switch(key_down) {
			case SDK_LEFT:
				moving = MOVE_LEFT;
				break;

			case SDK_RIGHT:
				moving = MOVE_RIGHT;
				break;

			default:
				SDK_get_mouse(&buttons, &x, &y);

				if (buttons & SDK_MOUSE_LEFT) {
					scroll_wheel = 0;
					key_down = 0;

					if (click_rect(&left_side, x, y))
						moving = MOVE_RIGHT;
					else
						if (click_rect(&right_side, x, y))
							moving = MOVE_LEFT;
				}
		}
		if (moving)
			ticks_moving = SDK_ticks();
	}
	switch(moving) {
		case MOVE_LEFT:
			move_covers_left();
			draw();
			break;

		case MOVE_RIGHT:
			move_covers_right();
			draw();
			break;

		default:
			;
	}
	if (!moving) {
		if (scroll_wheel > 0) {
			scroll_wheel--;
			if (scroll_wheel > 0)
				moving = was_moving;
		}
/*
	moving may have been reset
	do exactly the same as above, but without resetting 'ticks_moving'
*/
		switch(key_down) {
			case SDK_LEFT:
				moving = MOVE_LEFT;
				break;

			case SDK_RIGHT:
				moving = MOVE_RIGHT;
				break;

			default:
				SDK_get_mouse(&buttons, &x, &y);

				if (buttons & SDK_MOUSE_LEFT) {
					scroll_wheel = 0;
					key_down = 0;

					if (click_rect(&left_side, x, y))
						moving = MOVE_RIGHT;
					else
						if (click_rect(&right_side, x, y))
							moving = MOVE_LEFT;
				}
		}
	}
}

/* EOB */
