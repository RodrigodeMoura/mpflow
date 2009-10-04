/*
	event.c	WJ109
*/

#include "event.h"
#include "SDK.h"
#include "main.h"
#include "SDL_syswm.h"
#include "cover.h"
#include "mpd.h"
#include "dirlist.h"
#include "texture.h"
#include "text.h"
#include "font.h"

#include <stdio.h>
#include <stdlib.h>


int key_down;
int moving;
unsigned int ticks_moving;

static int mouse_drag, window_drag, drag_x, drag_y, orig_x, orig_y;
static int lpress_x, lpress_y, rpress_x, rpress_y;
static int dir_x, dir_y, dir_x_change, dir_y_change;
static unsigned int center_clicked = 0;
static int scroll_wheel = 0;

static SDL_Rect left_corner;
static SDL_Rect right_corner;


void jump_to_cover(int key) {
	if (jump_cover(key) == -1)
		return;

	set_cover_dirlist();
	delete_textures(NUM_COVER_TEXTURES);
	create_textures(NUM_COVER_TEXTURES);
	load_cover_textures();

	draw();
}

void handle_keypress(int key) {
	if (mode == MODE_TITLE_SCREEN) {
		if (key == SDK_ESC)
			exit_program(0);
		return;
	}
	switch(key) {
		case SDK_ESC:
			exit_program(0);
			break;

		case SDK_ENTER:
			play_album(covers[CENTER_COVER].dirlist->path);
			break;

		case SDK_SPACE:

			play_pause();
			break;

		case SDK_BACKSPACE:
			play_next();
			break;

		case SDK_TAB:
			play_random();
			break;

		default:
			jump_to_cover(key);
	}
}

void key_event(SDK_Event state, int key) {
	if (state == SDK_PRESS) {
		key_down = key;
		handle_keypress(key);
	} else
		key_down = 0;
}

void move(void) {
	if (!moving) {
		switch(key_down) {
			case SDK_LEFT:
				moving = MOVE_LEFT;
				ticks_moving = SDK_ticks();
				break;

			case SDK_RIGHT:
				moving = MOVE_RIGHT;
				ticks_moving = SDK_ticks();
				break;

			default:
				;
		}
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
/* moving may have been reset */
	if (!moving) {
		if (scroll_wheel > 0) {
			scroll_wheel--;
			if (!scroll_wheel) {
				key_down = 0;
				return;
			}
		}
		switch(key_down) {
			case SDK_LEFT:
				moving = MOVE_LEFT;
				break;

			case SDK_RIGHT:
				moving = MOVE_RIGHT;
				break;

			default:
				;
		}
	}
}

/*
	see if a mouse click was in a particular area
*/
static int click_rect(SDL_Rect *r, int x, int y) {
	if (x < r->x)
		return 0;

	if (x > r->x + r->w)
		return 0;

	if (y < r->y)
		return 0;

	if (y > r->y + r->h)
		return 0;

	return 1;
}

/*
	get absolute screen coordinates for the mouse position
*/
void get_abs_mouse(int *mouse_x, int *mouse_y) {
SDL_SysWMinfo info;

	SDL_VERSION(&info.version);

	if (SDL_GetWMInfo(&info) > 0 && info.subsystem == SDL_SYSWM_X11) {
		Window root_window, child_window;
		int a, b;
		unsigned int u;

		info.info.x11.lock_func();

		XQueryPointer(info.info.x11.display, info.info.x11.wmwindow, &root_window, &child_window, mouse_x, mouse_y, &a, &b, &u);

		info.info.x11.unlock_func();
	}
}

/*
	SDL mouse position (x, y) is relative to application window, so from (0, 0) to (window_width, window_height)
	for window dragging, this is not good enough, because the moving the window and reading the relative mouse position
	is causing "jumps" in mouse coordinates (due to 'resonation'?)
	To fix this, read the absolute screen coordinates for the mouse instead. SDL can not do this;
	use Xlib function XQueryPointer() instead
	Window dragging is now rock solid :)
*/
void mouse_event(SDK_Event event, int buttons, int x, int y) {
	switch(event) {
		case SDK_PRESS:
			if (mode == MODE_TITLE_SCREEN)
				break;

			if (buttons & SDK_MOUSE_LEFT) {
				lpress_x = x;
				lpress_y = y;

				if (y <= screen_height / 8) {			/* top of window activates window drag */
					window_drag = 1;
					get_abs_mouse(&drag_x, &drag_y);
					orig_x = drag_x;
					orig_y = drag_y;
					dir_x = dir_y = dir_x_change = dir_y_change = 0;
				} else {
					if (x < screen_width / 3) {
						key_down = SDK_RIGHT;
						mouse_drag = 1;
					} else {
						if (x > screen_width - screen_width / 3) {
							key_down = SDK_LEFT;
							mouse_drag = 1;
						}
					}
				}
			}
			if (buttons & SDK_MOUSE_RIGHT) {
				rpress_x = x;
				rpress_y = y;
			}
			if (buttons & SDK_MOUSE_WHEELUP) {
				if (key_down != SDK_RIGHT)
					scroll_wheel++;

				key_down = SDK_LEFT;
			}
			if (buttons & SDK_MOUSE_WHEELDOWN) {
				if (key_down != SDK_LEFT)
					scroll_wheel++;

				key_down = SDK_RIGHT;
			}
			break;

		case SDK_RELEASE:
			if (mode == MODE_TITLE_SCREEN) {			/* switch back from title screen */
				mode = MODE_DEFAULT;
				clear_text();
				reset_cover_title_text();
				draw();
				break;
			}
			if (buttons & SDK_MOUSE_LEFT) {
				window_drag = mouse_drag = 0;
				key_down = 0;

/* shake window for random play */
				if (dir_x_change + dir_y_change >= WINDOW_SHAKE)
					play_random();

				dir_x = dir_y = dir_x_change = dir_y_change = 0;

/* double click on center cover plays the album */
				if (click_rect(&center_cover, x, y) && click_rect(&center_cover, lpress_x, lpress_y)) {
					if (SDK_ticks() - center_clicked <= MOUSE_DOUBLECLICK)
						play_album(covers[CENTER_COVER].dirlist->path);
					else
/* single click pauses */
						play_pause();

					center_clicked = SDK_ticks();
				} else {
/* click in top corners to activate title screen */
					if ((click_rect(&left_corner, x, y) && click_rect(&left_corner, lpress_x, lpress_y))
						|| (click_rect(&right_corner, x, y) && click_rect(&right_corner, lpress_x, lpress_y))) {
						mode = MODE_TITLE_SCREEN;
						clear_text();
						center_text(-1, screen_height - FONT_H * 2, "mpflow Copyright (C) 2009 Walter de Jong <walter@heiho.net>", YELLOW);
						draw();
						break;
					}
				}
			}
/* right click: skip; play next song */
			if (buttons & SDK_MOUSE_RIGHT) {
				if (click_rect(&center_cover, x, y) && click_rect(&center_cover, rpress_x, rpress_y))
					play_next();
			}
			break;

		case SDK_MOUSEMOVE:
			if (mode == MODE_TITLE_SCREEN)
				break;

			if (window_drag) {
				int new_x, new_y;

				get_abs_mouse(&new_x, &new_y);

/* keep stats about directional changes for detecting 'window shake' */
				if (new_x - orig_x < 0) {
					if (dir_x >= 0)
						dir_x_change++;
					dir_x = -1;
				} else {
					if (new_x - orig_x > 0) {
						if (dir_x <= 0)
							dir_x_change++;
						dir_x = 1;
					}
				}
				if (new_y - orig_y < 0) {
					if (dir_y >= 0)
						dir_y_change++;
					dir_y = -1;
				} else {
					if (new_y - orig_y > 0) {
						if (dir_y <= 0)
							dir_y_change++;
						dir_y = 1;
					}
				}

/* drag window to new position */
				move_app_window(window_x + (new_x - drag_x), window_y + (new_y - drag_y));
				drag_x = new_x;
				drag_y = new_y;
			} else {
				if (mouse_drag) {
					int k;

					if (x < screen_width / 2)
						k = SDK_RIGHT;
					else
						k = SDK_LEFT;

					if (k != key_down) {
						key_down = k;
						ticks_moving = SDK_ticks();
					}
				}
			}
			break;

		default:
			;
	}
}

void window_event(SDK_Event event, int w, int h) {
	switch(event) {
		case SDK_CLOSE:
			exit_program(0);
			break;

		case SDK_EXPOSE:
			draw();
			break;

		default:
			;
	}
}

void init_events(void) {
	key_down = 0;
	moving = 0;
	ticks_moving = 0;

	mouse_drag = window_drag = 0;

	left_corner.x = left_corner.y = right_corner.y = 0;
	left_corner.w = right_corner.w = screen_width / 10;
	left_corner.h = right_corner.h = screen_height / 10;
	right_corner.x = screen_width - right_corner.w;

	SDK_key_event(key_event);
	SDK_mouse_event(mouse_event);
	SDK_window_event(window_event);
}

/* EOB */
