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
#include "widget.h"
#include "widget_covers.h"
#include "widget_about.h"

#include <stdio.h>
#include <stdlib.h>


int key_down = 0;
int moving = 0;
int scroll_wheel = 0;
unsigned int ticks_moving;

static int mouse_drag, window_drag, drag_x, drag_y, orig_x, orig_y;
static int lpress_x, lpress_y, rpress_x, rpress_y;
static unsigned int lpress_time = 0, rpress_time = 0;
static int dir_x, dir_y, dir_x_change, dir_y_change;


void jump_to_cover(int key) {
	if (jump_cover(key) == -1)
		return;

	set_cover_dirlist();
	delete_textures(NUM_COVER_TEXTURES);
	create_textures(NUM_COVER_TEXTURES);
	load_cover_textures();

	draw();
}

void key_event(SDK_Event state, int key) {
	if (state == SDK_PRESS) {
		key_down = key;
		input_widgets(key);
	} else
		key_down = 0;
}

/*
	see if a mouse click was in a particular area
*/
int click_rect(SDL_Rect *r, int x, int y) {
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
			if (buttons & SDK_MOUSE_LEFT) {
				lpress_x = x;
				lpress_y = y;
				lpress_time = SDK_ticks();

				get_abs_mouse(&drag_x, &drag_y);
				orig_x = x;
				orig_y = y;
			}
			if (buttons & SDK_MOUSE_RIGHT) {
				rpress_x = x;
				rpress_y = y;
				rpress_time = SDK_ticks();
			}
/* pass on scroll wheel events */
			if (buttons & (SDK_MOUSE_WHEELUP|SDK_MOUSE_WHEELDOWN))
				mouse_widgets(event, buttons, x, y);

			break;

		case SDK_RELEASE:
			window_drag = mouse_drag = 0;

			if (buttons & SDK_MOUSE_LEFT) {
				lpress_time = SDK_ticks() - lpress_time;
				if (abs(lpress_x - x) <= 2 && abs(lpress_y - y) <= 2 && lpress_time <= MOUSE_CLICK) {
					lpress_time = 0;
					click_widgets(SDK_MOUSE_LEFT, x, y);
					break;
				}
				lpress_time = 0;
			}
			if (buttons & SDK_MOUSE_RIGHT) {
				rpress_time = SDK_ticks() - rpress_time;
				if (abs(rpress_x - x) <= 2 && abs(rpress_y - y) <= 2 && rpress_time <= MOUSE_CLICK) {
					rpress_time = 0;
					click_widgets(SDK_MOUSE_RIGHT, x, y);
					break;
				}
				rpress_time = 0;
			}
			break;

		case SDK_MOUSEMOVE:
			if (lpress_time) {
				int new_x, new_y;

				get_abs_mouse(&new_x, &new_y);

				drag_widgets(orig_x, orig_y, new_x - drag_x, new_y - drag_y);

				drag_x = new_x;
				drag_y = new_y;
			}

#ifdef OLD_CODE
/* dit is voor window shake */
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
			}
#endif
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

	SDK_key_event(key_event);
	SDK_mouse_event(mouse_event);
	SDK_window_event(window_event);
}

/* EOB */
