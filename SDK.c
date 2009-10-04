/*
	SDK.c	WJ109

	- wrapper around SDL;
	SDL is great but I want to abstract it so I may drop in a replacement more easily
*/

#include "SDK.h"

#include <stdio.h>
#include <stdlib.h>


static int max_xres;
static int max_yres;

static void (*window_events)(SDK_Event, int, int) = NULL;
static void (*key_events)(SDK_Event, int) = NULL;
static void (*mouse_events)(SDK_Event, int, int, int) = NULL;

static int mouse_buttons;
static int mouse_x;
static int mouse_y;

static int key_state;
static int translate_keys;

static Uint32 window_hints;


void SDK_init(void) {
Uint8 buttons;
SDLMod mod;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDK_init() failed\n");
		exit(-1);
	}
/* max video mode resolution */
	max_xres = max_yres = 99999;

/* event handlers */
	window_events = NULL;
	key_events = NULL;
	mouse_events = NULL;

/*
	init mouse state
*/
	mouse_buttons = 0;
	buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
	if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
		mouse_buttons |= SDK_MOUSE_LEFT;

	if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
		mouse_buttons |= SDK_MOUSE_RIGHT;

	if (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE))
		mouse_buttons |= SDK_MOUSE_MIDDLE;

/* by default, disable key repeat */
	SDK_keyrepeat(0);

/* init key_state */
	key_state = 0;

	mod = SDL_GetModState();

	if (mod & KMOD_LCTRL)
		key_state |= SDK_LCTRL_DOWN;

	if (mod & KMOD_RCTRL)
		key_state |= SDK_RCTRL_DOWN;

	if (mod & KMOD_LSHIFT)
		key_state |= SDK_LSHIFT_DOWN;

	if (mod & KMOD_RSHIFT)
		key_state |= SDK_RSHIFT_DOWN;

	if (mod & KMOD_LALT)
		key_state |= SDK_LALT_DOWN;

	if (mod & KMOD_RALT)
		key_state |= SDK_RALT_DOWN;

	window_hints = 0;
}

void SDK_exit(int code) {
	SDL_Quit();
	exit(code);
}

void SDK_max_videomode(int *w, int *h) {
SDL_Rect **modes;

	*w = *h = -1;

	if ((modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_OPENGL|SDL_GL_DOUBLEBUFFER)) == NULL) {
		fprintf(stderr, "warning: failed to list video modes\n");
		return;
	}
	if (modes == (SDL_Rect **)-1) {			/* any mode will do */
		*w = *h = 0;
		return;
	}
/*
	int i;

	printf("available video modes:\n");
	for(i = 0; modes[i] != NULL; i++)
		printf("  %dx%d\n", modes[i]->w, modes[i]->h);
*/
	*w = max_xres = modes[0]->w;
	*h = max_yres = modes[0]->h;
}

void SDK_window_hints(int hints) {
	window_hints = hints;
}

int SDK_create_window(int w, int h, int flags, char *title) {
Uint32 modeflags;

	modeflags = SDL_OPENGL|SDL_HWSURFACE|window_hints;

	if (flags & SDK_FULLSCREEN)
		modeflags |= SDL_FULLSCREEN;
	else
		modeflags |= SDL_RESIZABLE;
/*
	NB. this is hardcoded, so screen depth is always 32 bits
*/
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	if (SDL_SetVideoMode(w, h, 32, modeflags) == NULL)
		return -1;

	SDL_WM_SetCaption(title, title);
	return 0;
}

void SDK_resize_window(int w, int h) {
Uint32 modeflags;

	modeflags = SDL_OPENGL|SDL_HWSURFACE;

	if (w >= max_xres && h >= max_yres)
		modeflags |= SDL_FULLSCREEN;
	else
		modeflags |= SDL_RESIZABLE;

	SDL_SetVideoMode(w, h, 32, modeflags);
}

/*
	NB. These need to be high-performance, so they are now #defines in SDK.h

void SDK_swapbuffers(void) {
	SDL_GL_SwapBuffers();
}

unsigned int SDK_ticks(void) {
	return (unsigned int)SDL_GetTicks();
}

void SDK_sleep(unsigned int msecs) {
	SDL_Delay((Uint32)msecs);
}
*/

void SDK_window_event(void (*handler)(SDK_Event, int, int)) {
	window_events = handler;
}

void SDK_key_event(void (*handler)(SDK_Event, int)) {
	key_events = handler;
}

void SDK_mouse_event(void (*handler)(SDK_Event, int, int, int)) {
	mouse_events = handler;
}


static void update_key_state(SDK_Event event, SDLKey key) {
int bit;

	bit = 0;
	switch(key) {
		case SDLK_RCTRL:
			bit = SDK_RCTRL_DOWN;
			break;

		case SDLK_LCTRL:
			bit = SDK_LCTRL_DOWN;
			break;

		case SDLK_RSHIFT:
			bit = SDK_RSHIFT_DOWN;
			break;

		case SDLK_LSHIFT:
			bit = SDK_LSHIFT_DOWN;
			break;

		case SDLK_RALT:
			bit = SDK_RALT_DOWN;
			break;

		case SDLK_LALT:
			bit = SDK_LALT_DOWN;
			break;

		default:
			;
	}
	if (event == SDK_PRESS)
		key_state |= bit;
	else
		key_state &= ~bit;
}

static int mouse_button_bit(int translate_button) {
	switch(translate_button) {
		case SDL_BUTTON_LEFT:
			return SDK_MOUSE_LEFT;

		case SDL_BUTTON_RIGHT:
			return SDK_MOUSE_RIGHT;

		case SDL_BUTTON_MIDDLE:
			return SDK_MOUSE_MIDDLE;

		case SDL_BUTTON_WHEELUP:
			return SDK_MOUSE_WHEELUP;

		case SDL_BUTTON_WHEELDOWN:
			return SDK_MOUSE_WHEELDOWN;

		default:
			;
	}
	return 0;
}

/*
	translate events to SDK_Events and call handlers
*/
void SDK_handle_events(void) {
SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				if (window_events != NULL)
					window_events(SDK_CLOSE, 0, 0);
				break;

			case SDL_VIDEOEXPOSE:
				if (window_events != NULL)
					window_events(SDK_EXPOSE, 0, 0);
				break;

			case SDL_VIDEORESIZE:
				if (window_events != NULL)
					window_events(SDK_RESIZE, event.resize.w, event.resize.h);
				break;

			case SDL_ACTIVEEVENT:				/* window lost focus */
				if (window_events == NULL)
					break;

				if (!event.active.gain) {
					SDK_show_mouse(1);
					window_events(SDK_INACTIVE, 0, 0);
				} else
					window_events(SDK_ACTIVE, 0, 0);

/* iconify (doesn't work in GNOME) */
				if (event.active.state == SDL_APPACTIVE && !event.active.gain)
					SDL_WaitEvent(NULL);		/* freeze it */

				break;

			case SDL_KEYDOWN:
				update_key_state(SDK_PRESS, event.key.keysym.sym);
				if (key_events != NULL)
					key_events(SDK_PRESS, (translate_keys) ? event.key.keysym.unicode : event.key.keysym.sym);
				break;

			case SDL_KEYUP:
				update_key_state(SDK_RELEASE, event.key.keysym.sym);
				if (key_events != NULL)
					key_events(SDK_RELEASE, (translate_keys) ? event.key.keysym.unicode : event.key.keysym.sym);
				break;

			case SDL_MOUSEBUTTONDOWN:
				if (mouse_events != NULL) {
					mouse_buttons = mouse_button_bit(event.button.button);
					mouse_events(SDK_PRESS, mouse_buttons, mouse_x, mouse_y);
				}
				break;

			case SDL_MOUSEBUTTONUP:
				if (mouse_events != NULL) {
					mouse_buttons = mouse_button_bit(event.button.button);
					mouse_events(SDK_RELEASE, mouse_buttons, mouse_x, mouse_y);
				}
				break;

			case SDL_MOUSEMOTION:
				if (mouse_events != NULL) {
					mouse_x = event.motion.x;
					mouse_y = event.motion.y;
					mouse_events(SDK_MOUSEMOVE, mouse_buttons, mouse_x, mouse_y);
				}
				break;

			default:
				;
		}
	}
}

void SDK_keyrepeat(int on_off) {
	if (on_off) {
		SDL_EnableKeyRepeat(SDK_KEYB_REPEAT_DELAY, SDK_KEYB_REPEAT_INTERVAL);
		SDL_EnableUNICODE(1);
		translate_keys = 1;
	} else {
		SDL_EnableKeyRepeat(0, 0);
		SDL_EnableUNICODE(0);
		translate_keys = 0;
	}
}

int SDK_key_state(void) {
	return key_state;
}

void SDK_show_mouse(int on_off) {
	if (on_off)
		SDL_ShowCursor(SDL_ENABLE);
	else
		SDL_ShowCursor(SDL_DISABLE);
}

void SDK_get_mouse(int *buttons, int *x, int *y) {
	*buttons = mouse_buttons;
	*x = mouse_x;
	*y = mouse_y;
}

/* EOB */
