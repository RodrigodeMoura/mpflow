/*
	mpflow	WJ109
*/

#include "main.h"
#include "SDK.h"
#include "SDL_image.h"
#include "SDL_syswm.h"
#include "glut.h"
#include "cover.h"
#include "event.h"
#include "mpd.h"
#include "font.h"
#include "text.h"
#include "texture.h"
#include "app_icon.h"
#include "widget_covers.h"
#include "widget_about.h"
#include "widget_aboutbutton.h"
#include "widget_titlebar.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define VERSION		"0.9.2"


int screen_width = 600;					/* current display resolution (or window size) */
int screen_height = 200;
int window_x, window_y;					/* window position */

int max_xres = 320, max_yres = 200;		/* maximum display resolution (as reported by SDK_max_videomode()) */
int display_xres, display_yres;			/* current display resolution */


void draw(void);


void exit_program(int code) {
	deinit_textures();
	SDK_exit(code);
}

void init_gl(void) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)ARENA_WIDTH / (float)ARENA_HEIGHT, 0.01f, 1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);			/* GL_TRUE */

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

/* simply always turn this on */
	glEnableClientState(GL_VERTEX_ARRAY);
}

/*
	get current display resolution from X11
*/
void get_display_res(int *w, int *h) {
SDL_SysWMinfo info;

	*w = *h = -1;

	SDL_VERSION(&info.version);

	if (SDL_GetWMInfo(&info) > 0 && info.subsystem == SDL_SYSWM_X11) {
		XWindowAttributes attrs;
		Window root, parent, *children;
		unsigned int n;

		info.info.x11.lock_func();

/* find the root window */
		if (XQueryTree(info.info.x11.display, info.info.x11.wmwindow, &root, &parent, &children, &n)) {
			if (children != NULL)
				XFree(children);

/* get width and height of the root window */
			if (XGetWindowAttributes(info.info.x11.display, root, &attrs)) {
				*w = attrs.width;
				*h = attrs.height;
			}
		}
		info.info.x11.unlock_func();
	}
}

void create_window(void) {
SDL_SysWMinfo info;
int xres, yres;

	get_display_res(&xres, &yres);

	screen_width = xres * 0.6f;
	screen_height = screen_width * 0.4f;

	SDK_window_hints(SDL_NOFRAME);

	if (SDK_create_window(screen_width, screen_height, 0, "mpflow") < 0) {
		fprintf(stderr, "error: failed to create window\n");
		exit_program(1);
	}
/*
	put window in center of the screen
*/
	SDL_VERSION(&info.version);

	if (SDL_GetWMInfo(&info) > 0 && info.subsystem == SDL_SYSWM_X11) {
/*
	This code gets the current video mode by reading the size of the desktop root window
	The trouble is that it needs a window to be able to do that

		Window root_window, child_window;
		int a, b, c, d;
		unsigned int u;
		XWindowAttributes attrs;

		info.info.x11.lock_func();

		XQueryPointer(info.info.x11.display, info.info.x11.window, &root_window, &child_window, &a, &b, &c, &d, &u);
		XGetWindowAttributes(info.info.x11.display, root_window, &attrs);

		info.info.x11.unlock_func();
		printf("TD root window size [%d, %d]\n", attrs.width, attrs.height);
*/
/* center window on screen */
		window_x = (xres - screen_width) / 2;
		window_y = (yres - screen_height) / 2;

		info.info.x11.lock_func();

		XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, window_x, window_y);
		XMapRaised(info.info.x11.display, info.info.x11.wmwindow);

		info.info.x11.unlock_func();
	} else
		window_x = window_y = 0;
}

/*
	move application to screen coordinates (x, y)
	This is used for window dragging
*/
void move_app_window(int x, int y) {
SDL_SysWMinfo info;

	SDL_VERSION(&info.version);

	if (SDL_GetWMInfo(&info) > 0 && info.subsystem == SDL_SYSWM_X11) {
		XWindowAttributes attrs;

		info.info.x11.lock_func();

		if (XGetWindowAttributes(info.info.x11.display, info.info.x11.wmwindow, &attrs)) {
			XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
/* assume XMoveWindow call succeeded :P */
			window_x = x;
			window_y = y;
		}
		info.info.x11.unlock_func();
	}
}

/*
	draw white frame around window
*/
void draw_window_border(void) {
GLfloat vertex_arr[8] = {
	0, 0,
	ARENA_WIDTH, 0,
	ARENA_WIDTH, ARENA_HEIGHT,
	0, ARENA_HEIGHT
};

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
#ifdef OPENGLES
	glOrthof(0.0f, ARENA_WIDTH, 0.0f, ARENA_HEIGHT, -Z_FAR, Z_FAR);
#else
	glOrtho(0.0f, ARENA_WIDTH, 0.0f, ARENA_HEIGHT, -Z_FAR, Z_FAR);
#endif
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1, 1, 1, 1);
	glLineWidth(4.0f);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-ARENA_WIDTH * 0.5f, -ARENA_HEIGHT * 0.5f, -180);

	draw_widgets();
	draw_window_border();

	glFlush();
	SDK_swapbuffers();
}

void draw_startup(void) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-ARENA_WIDTH * 0.5f, -ARENA_HEIGHT * 0.5f, -180);

	draw_window_border();

	glFlush();
	SDK_swapbuffers();
}

/*
	get lock on lock file
*/
int lock_program(void) {
int fd;
struct flock lock;

	if ((fd = open("/var/lock/mpflow.lock", O_WRONLY|O_CREAT, 0644)) == -1)
		return -1;

	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 1;

	if (fcntl(fd, F_SETLK, &lock) == -1)
		return -1;

/* fd remains open */
	return 0;
}

void get_options(int argc, char *argv[]) {
int i;

	if (argc <= 1)
		return;

	if (!strcmp(argv[1], "-?") || !strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
		printf("\n"
			"mpflow is a front-end to mpd (Music Player Daemon) that displays the album art\n"
			"that is present in your music directory. It searches for these files:\n");

		i = 0;
		while(album_art[i] != NULL)
			printf("  %s\n", album_art[i++]);

		printf("\n"
			"usage:\n"
			"- use mouse or cursor keys to flip through your albums\n");
		printf("- double click the center album to play it\n"
			"- click once to pause playback\n");
		printf("- right click to skip to the next song\n"
			"- shake the window to shuffle playback\n");
		printf("- press first letter of an album to skip through the list\n");
		printf("- hit escape to exit mpflow\n");
		printf("\n"
			"Note that mpflow needs mpd and can not be used standalone.\n");

		printf("\n\n");
		printf("mpflow COMES WITH NO WARRANTY. mpflow IS FREE SOFTWARE.\n"
			"mpflow is distributed under terms described in the GNU General Public\n"
			"License.\n");

		exit(1);
	}
	if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version")) {
		printf("This is version " VERSION "\n");
		exit(0);
	}
	if (argv[1][0] == '-')
		printf("unknown command line option '%s'\n", argv[1]);
	else
		printf("invalid command line argument '%s'\n", argv[1]);

	exit(1);
}

int main(int argc, char *argv[]) {
	printf("mpflow - Copyright (C) 2009 Walter de Jong <walter@heiho.net>\n");
	get_options(argc, argv);

	if (lock_program() == -1) {
		printf("another instance of mpflow is already running\n");
		return 1;
	}
	init_mpd();

/* init app */
	SDK_init();

	create_window();
	set_app_icon();

	init_gl();
	draw_startup();			/* only a border */

/* note that the order of init_() functions here is important (!) */
	init_textures();
	init_text();
	init_covers();
	init_events();

	init_widget_covers();
	init_widget_about();
	init_widget_aboutbutton();
	init_widget_titlebar();

	main_widget = &w_titlebar;
	prepare_widgets();
	draw();

	for(;;) {
		SDK_handle_events();
		move_covers();

		if (!moving)
			SDL_WaitEvent(NULL);
		else
			SDK_sleep(FRAME_DELAY);
	}
	exit_program(0);
	return 0;
}

/* EOB */
