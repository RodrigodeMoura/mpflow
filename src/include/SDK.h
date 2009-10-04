/*
	SDK.h	WJ109
*/

#ifndef SDK_H_WJ109
#define SDK_H_WJ109	1

#include "SDL.h"
#include "SDL_opengl.h"

/* defined functions */
#define SDK_swapbuffers		SDL_GL_SwapBuffers
#define SDK_ticks			(unsigned int)SDL_GetTicks
#define SDK_sleep(x)		SDL_Delay((Uint32)(x))

/* video mode flags */
#define SDK_FULLSCREEN	1

/* event codes */
typedef enum {
	SDK_INVALID = 0,

	SDK_CLOSE,
	SDK_EXPOSE,
	SDK_RESIZE,
	SDK_ACTIVE,
	SDK_INACTIVE,

	SDK_PRESS,
	SDK_RELEASE,

	SDK_MOUSEMOVE,
} SDK_Event;

/* mouse button bits */
#define SDK_MOUSE_LEFT			1
#define SDK_MOUSE_RIGHT			2
#define SDK_MOUSE_MIDDLE		4
#define SDK_MOUSE_WHEELUP		8
#define SDK_MOUSE_WHEELDOWN		0x10

/* key repeat values */
#define SDK_KEYB_REPEAT_DELAY		100			/* milliseconds */
#define SDK_KEYB_REPEAT_INTERVAL	40			/* milliseconds */

/* keys */
#define SDK_ENTER		SDLK_RETURN
#define SDK_ESC			SDLK_ESCAPE
#define SDK_SPACE		SDLK_SPACE
#define SDK_PAUSE		SDLK_PAUSE
#define SDK_TAB			SDLK_TAB
#define SDK_BACKSPACE	SDLK_BACKSPACE
#define SDK_DEL			SDLK_DELETE
#define SDK_UP			SDLK_UP
#define SDK_DOWN		SDLK_DOWN
#define SDK_LEFT		SDLK_LEFT
#define SDK_RIGHT		SDLK_RIGHT
#define SDK_INS			SDLK_INSERT
#define SDK_HOME		SDLK_HOME
#define SDK_END			SDLK_END
#define SDK_PGUP		SDLK_PAGEUP
#define SDK_PGDN		SDLK_PAGEDOWN
#define SDK_LSHIFT		SDLK_LSHIFT
#define SDK_RSHIFT		SDLK_RSHIFT
#define SDK_LCTRL		SDLK_LCTRL
#define SDK_RCTRL		SDLK_RCTRL
#define SDK_LALT		SDLK_LALT
#define SDK_RALT		SDLK_RALT

/* key state bits */
#define SDK_LCTRL_DOWN		1
#define SDK_RCTRL_DOWN		2
#define SDK_CTRL_DOWN		(SDK_LCTRL_DOWN|SDK_RCTRL_DOWN)
#define SDK_LSHIFT_DOWN		4
#define SDK_RSHIFT_DOWN		8
#define SDK_SHIFT_DOWN		(SDK_LSHIFT_DOWN|SDK_RSHIFT_DOWN)
#define SDK_LALT_DOWN		0x10
#define SDK_RALT_DOWN		0x20
#define SDK_ALT_DOWN		(SDK_LALT_DOWN|SDK_RALT_DOWN)


void SDK_init(void);
void SDK_exit(int);
void SDK_max_videomode(int *, int *);
void SDK_window_hints(int);
int SDK_create_window(int, int, int, char *);
void SDK_resize_window(int, int);
void SDK_window_event(void (*)(SDK_Event, int, int));
void SDK_key_event(void (*)(SDK_Event, int));
void SDK_mouse_event(void (*)(SDK_Event, int, int, int));
void SDK_handle_events(void);
void SDK_keyrepeat(int);
int SDK_key_state(void);
void SDK_show_mouse(int);
void SDK_get_mouse(int *, int *, int *);

#endif	/* SDK_H_WJ109 */

/* EOB */
