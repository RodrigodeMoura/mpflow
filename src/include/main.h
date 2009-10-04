/*
	mpflow	WJ109
*/

#ifndef MAIN_H_WJ109
#define MAIN_H_WJ109	1

#define APP_ICON		"mpflow.png"

#define ARENA_WIDTH		400
#define ARENA_HEIGHT	160
#define Z_FAR			160

#define FPS				50
#define FRAME_DELAY		(1000 / FPS)

typedef enum {
	MODE_DEFAULT = 0,
	MODE_TITLE_SCREEN = 1
} ProgramMode;

extern int screen_width;
extern int screen_height;
extern int window_x;
extern int window_y;
extern ProgramMode mode;

void exit_program(int);
void move_app_window(int, int);
void draw(void);

#endif	/* MAIN_H_WJ109 */

/* EOB */
