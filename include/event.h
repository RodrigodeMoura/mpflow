/*
	event.h	WJ109
*/

#ifndef EVENT_H_WJ109
#define EVENT_H_WJ109	1

#define MOVE_LEFT		1
#define MOVE_RIGHT		2

#define MOUSE_DOUBLECLICK	500			/* time in milliseconds */
#define WINDOW_SHAKE		5

extern int key_down;
extern int moving;
extern unsigned int ticks_moving;

void init_events(void);
void move(void);

#endif	/* EVENT_H_WJ109 */

/* EOB */
