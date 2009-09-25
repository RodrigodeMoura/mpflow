/*
	event.c	WJ109
*/

/* TODO mouse drag window maken */

#include "event.h"
#include "SDK.h"
#include "main.h"
#include "SDL_syswm.h"
#include "cover.h"

#include <stdio.h>
#include <stdlib.h>


int key_down;
int moving;
unsigned int ticks_moving;

static int mouse_drag, drag_x, drag_y;


void handle_keypress(int key) {
	switch(key) {
		case SDK_ESC:
			SDK_exit(0);
			break;

		default:
			;
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
/* TODO increase flipping speed to a certain maximum */
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
			if (y <= screen_height / 8) {			/* top of window activates window drag */
				mouse_drag = 1;
				get_abs_mouse(&drag_x, &drag_y);
			}
			break;

		case SDK_RELEASE:
			mouse_drag = 0;
			break;

		case SDK_MOUSEMOVE:
			if (mouse_drag) {
				int new_x, new_y;

				get_abs_mouse(&new_x, &new_y);
				move_app_window(window_x + (new_x - drag_x), window_y + (new_y - drag_y));
				drag_x = new_x;
				drag_y = new_y;
			}
			break;

		default:
			;
	}
}

void window_event(SDK_Event event, int w, int h) {
	switch(event) {
		case SDK_CLOSE:
			SDK_exit(0);
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

	mouse_drag = 0;

	SDK_key_event(key_event);
	SDK_mouse_event(mouse_event);
	SDK_window_event(window_event);
}

/* EOB */
