/*
	mpflow	WJ109
*/

#ifndef MAIN_H_WJ109
#define MAIN_H_WJ109	1

#define ARENA_WIDTH		400
#define ARENA_HEIGHT	160
#define Z_FAR			160

#define FPS				30

extern int screen_width;
extern int screen_height;
extern int window_x;
extern int window_y;

void move_app_window(int, int);
void draw(void);

#endif	/* MAIN_H_WJ109 */

/* EOB */
