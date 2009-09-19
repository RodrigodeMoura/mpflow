/*
	mpflow	WJ109
*/

#include "main.h"
#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>


int screen_width = 600;					/* current display resolution (or window size) */
int screen_height = 200;

int max_xres = 320, max_yres = 200;		/* maximum display resolution (as reported by SDK_max_videomode()) */

int key_down;


void handle_keypress(int key) {
	switch(key) {
		case SDK_ESC:
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

void window_event(SDK_Event event, int w, int h) {
	switch(event) {
		case SDK_CLOSE:
			SDK_exit(0);
			break;

		default:
			;
	}
}

void create_window(void) {
	SDK_max_videomode(&max_xres, &max_yres);

	screen_width = max_xres * 0.6f;
	screen_height = max_yres * 0.2f;

	if (SDK_create_window(screen_width, screen_height, 0, "mpflow") < 0) {
		fprintf(stderr, "error: failed to create window\n");
		SDK_exit(1);
	}
}


int main(int argc, char *argv[]) {
	SDK_init();

	create_window();

	SDK_key_event(key_event);
	SDK_window_event(window_event);

	key_down = 0;
	while(key_down != SDK_ESC) {
		SDK_handle_events();
		SDK_sleep(200);
	}
	SDK_exit(0);
	return 0;
}

/* EOB */
