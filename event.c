/*
	event.c	WJ109
*/

/* TODO mouse drag window maken */

#include "event.h"
#include "SDK.h"
#include "main.h"
#include "SDL_syswm.h"

#include <stdio.h>
#include <stdlib.h>


int key_down;
int moving;

static int mouse_drag, drag_x, drag_y;


void handle_keypress(int key) {
	switch(key) {
		case SDK_ESC:
			SDK_exit(0);
			break;

		case SDK_LEFT:
			if (!moving)
				moving = MOVE_LEFT;
			break;

		case SDK_RIGHT:
			if (!moving)
				moving = MOVE_RIGHT;
			break;

		case SDK_BACKSPACE:
			move_app_window(-10, 0);
			break;

		case SDK_SPACE:
			move_app_window(10, 0);
			break;

		default:
			;
	}
}

void handle_keyrelease(int key) {
	switch(key) {
		case SDK_LEFT:
			break;

		case SDK_RIGHT:
			break;

		default:
			;
	}
}

void key_event(SDK_Event state, int key) {
	if (state == SDK_PRESS) {
		key_down = key;
		handle_keypress(key);
	} else {
		key_down = 0;
		handle_keyrelease(key);
	}
}

/*
	mouse position (x, y) is relative to application window, so from (0, 0) to (window_width, window_height)
*/
void mouse_event(SDK_Event event, int buttons, int x, int y) {
SDL_SysWMinfo info;
Window root_window, child_window;
int a, b;
unsigned int u;

	switch(event) {
		case SDK_PRESS:
			printf("TD mouse press at (%d, %d)\n", x, y);
			mouse_drag = 1;

			SDL_VERSION(&info.version);

			if (SDL_GetWMInfo(&info) > 0 && info.subsystem == SDL_SYSWM_X11) {
				info.info.x11.lock_func();

				XQueryPointer(info.info.x11.display, info.info.x11.wmwindow, &root_window, &child_window, &drag_x, &drag_y, &a, &b, &u);

				info.info.x11.unlock_func();
			}
			printf("TD mouse abs [%d, %d]\n", drag_x, drag_y);
			break;

		case SDK_RELEASE:
			mouse_drag = 0;
			break;

		case SDK_MOUSEMOVE:
			if (mouse_drag) {
				int new_x, new_y;

				SDL_VERSION(&info.version);

				if (SDL_GetWMInfo(&info) > 0 && info.subsystem == SDL_SYSWM_X11) {
					info.info.x11.lock_func();

					XQueryPointer(info.info.x11.display, info.info.x11.wmwindow, &root_window, &child_window, &new_x, &new_y, &a, &b, &u);

					info.info.x11.unlock_func();
				}
				printf("TD mouse abs [%d, %d]\n", window_x + x, window_y + y);
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

	mouse_drag = 0;

	SDK_key_event(key_event);
	SDK_mouse_event(mouse_event);
	SDK_window_event(window_event);
}

/* EOB */
