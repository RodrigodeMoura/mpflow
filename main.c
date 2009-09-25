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
#include "mpdconf.h"
#include "inet.h"
#include "dirlist.h"

#include <stdio.h>
#include <stdlib.h>


int screen_width = 600;					/* current display resolution (or window size) */
int screen_height = 200;
int window_x, window_y;					/* window position */

int max_xres = 320, max_yres = 200;		/* maximum display resolution (as reported by SDK_max_videomode()) */
int display_xres, display_yres;			/* current display resolution */


void draw(void);


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
		SDK_exit(1);
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

void set_app_icon(void) {
SDL_Surface *icon;

	if ((icon = IMG_Load(APP_ICON)) == NULL)
		return;

	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);
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

	draw_covers();
	draw_window_border();

	glFlush();
	SDK_swapbuffers();
}

int connect_mpd(void) {
int sock;
char buf[1280], *rest;

	sock = inet_connect(config_address, config_port);

/* MPD announces itself; read first line */
	rest = NULL;
	if (inet_readline(sock, buf, &rest, sizeof(buf)) == NULL) {
		fprintf(stderr, "error in connection to MPD\n");
		inet_close(sock);
		return -1;
	}
	if (strncmp(buf, "OK MPD ", 7)) {
		fprintf(stderr, "error: the service we connected to does not look like MPD\n");
		inet_close(sock);
		return -1;
	}
/* authenticate */
	if (config_password != NULL) {
		snprintf(buf, sizeof(buf), "password %s\n", config_password);
		inet_write(sock, buf);

		rest = NULL;
		if (inet_readline(sock, buf, &rest, sizeof(buf)) == NULL) {
			fprintf(stderr, "error in connection to MPD\n");
			inet_close(sock);
			return -1;
		}
		if (strncmp(buf, "OK", 2)) {
			fprintf(stderr, "error: wrong password for MPD\n");
			inet_close(sock);
			return -1;
		}
	}
/* get all directories */
	inet_write(sock, "listall\n");

	rest = NULL;
	while(inet_readline(sock, buf, &rest, sizeof(buf)) != NULL) {
		if (!strncmp(buf, "directory: ", 11)) {
			add_directory(buf+11);
			continue;
		}
		if (!strcmp(buf, "OK")) {
			inet_write(sock, "close\n");
			continue;
		}
	}
	inet_close(sock);

	init_current_DirList();
	return 0;
}

int main(int argc, char *argv[]) {
	read_mpdconf();

	if (connect_mpd())
		return -1;

	SDK_init();

	create_window();
	set_app_icon();

	init_gl();
	init_covers();
	init_events();

	draw();

	for(;;) {
		SDK_handle_events();
		move();
		SDK_sleep(50);
	}
	return 0;
}

/* EOB */
