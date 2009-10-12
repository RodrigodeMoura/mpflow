/*
	event.h	WJ109
*/

#ifndef EVENT_H_WJ109
#define EVENT_H_WJ109	1

#include "SDL.h"

#define MOVE_LEFT		1
#define MOVE_RIGHT		2

#define MOUSE_CLICK			350		/* time in milliseconds */
#define MOUSE_DOUBLECLICK	600		/* time in milliseconds */
#define WINDOW_SHAKE		5

extern int key_down;
extern int moving;
extern int scroll_wheel;
extern unsigned int ticks_moving;

void init_events(void);
void move(void);
int click_rect(SDL_Rect *, int, int);

#endif	/* EVENT_H_WJ109 */

/* EOB */
