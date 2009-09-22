/*
	event.c	WJ109
*/

#include "event.h"
#include "SDK.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>


int key_down;
int moving;


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

	SDK_key_event(key_event);
	SDK_window_event(window_event);
}

/* EOB */
