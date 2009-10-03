/*
	cover.c	WJ109
*/

#include "cover.h"
#include "SDK.h"
#include "main.h"
#include "event.h"
#include "mpdconf.h"
#include "texture.h"
#include "glPrint.h"

#include <stdio.h>
#include <stdlib.h>


Cover covers[NUM_COVERS];

/*
	window coordinates and dimension of the center cover in pixels
	This is _not_ used by OpenGL, but is needed for the mouse
*/
SDL_Rect center_cover;


static void load_cover_texture(Cover *c) {
	find_album_art(c->dirlist);

	if (c->dirlist->img != NULL && c->dirlist->img[0]) {
		char filename[1024];

		snprintf(filename, sizeof(filename), "%s/%s/%s", config_musicdir, c->dirlist->path, c->dirlist->img);
		load_texture(c->texture_idx, filename);
	}
}

/*
	get the position & dimension of the center cover in screen coordinates
	This is needed for correct mouse handling
*/
static void get_cover_coords(void) {
GLdouble model_matrix[16], project_matrix[16], obj_x, obj_y, obj_z, win_x, win_y, win_z;
GLint viewport[4];

	center_cover.x = center_cover.y = center_cover.w = center_cover.h = 0;

	glLoadIdentity();
	glTranslatef(-ARENA_WIDTH * 0.5f, -ARENA_HEIGHT * 0.5f, -180);

	glGetDoublev(GL_MODELVIEW_MATRIX, model_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, project_matrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	obj_x = (ARENA_WIDTH - COVER_W) * 0.5f;
	obj_y = (ARENA_HEIGHT - COVER_H) * 0.5f;
	obj_z = 0.0f;

	win_x = win_y = win_z = 0.0f;

	if (gluProject(obj_x, obj_y, obj_z, model_matrix, project_matrix, viewport, &win_x, &win_y, &win_z) == GL_FALSE)
		fprintf(stderr, "error: gluProject() failed, unable to determine window coordinates for correct mouse handling\n");
	else {
		center_cover.x = (int)win_x;
		center_cover.y = (int)win_y;
	}
	obj_x += COVER_W;
	obj_y += COVER_H;

	if (gluProject(obj_x, obj_y, obj_z, model_matrix, project_matrix, viewport, &win_x, &win_y, &win_z) == GL_FALSE)
		fprintf(stderr, "error: gluProject() failed, unable to determine window coordinates for correct mouse handling\n");
	else {
		center_cover.w = (int)win_x - center_cover.x;
		center_cover.h = (int)win_y - center_cover.y;
	}
}

void init_covers(void) {
int i;
float xpos, ypos, color;
DirList *d;

/* left side */
	xpos = ARENA_WIDTH * 0.5f - CENTER_SPACE - 5 * COVER_DISTANCE;
	ypos = ARENA_HEIGHT * 0.5f;
	color = 0.5f;

	covers[0].x = xpos;
	covers[0].y = ypos;
	covers[0].z = COVER_Z;
	covers[0].angle = COVER_ANGLE;
	covers[0].color = color;
	covers[0].pos = 0;

	for(i = 1; i < CENTER_COVER; i++) {
		covers[i].x = xpos;
		covers[i].y = ypos;
		covers[i].z = COVER_Z;
		covers[i].angle = COVER_ANGLE;
		covers[i].color = color;
		covers[i].pos = i;

		xpos += COVER_DISTANCE;
		color += 0.1f;
	}

/* right side */
	xpos = ARENA_WIDTH * 0.5f + CENTER_SPACE + 5 * COVER_DISTANCE;
	color = 0.5f;

	covers[NUM_COVERS-1].x = xpos;
	covers[NUM_COVERS-1].y = ypos;
	covers[NUM_COVERS-1].z = COVER_Z;
	covers[NUM_COVERS-1].angle = -COVER_ANGLE;
	covers[NUM_COVERS-1].color = color;

	for(i = NUM_COVERS-2; i > CENTER_COVER; i--) {
		covers[i].x = xpos;
		covers[i].y = ypos;
		covers[i].z = COVER_Z;
		covers[i].angle = -COVER_ANGLE;
		covers[i].color = color;
		covers[i].pos = i;

		xpos -= COVER_DISTANCE;
		color += 0.1f;
	}

/* center cover */
	covers[CENTER_COVER].x = ARENA_WIDTH * 0.5f;
	covers[CENTER_COVER].y = ypos;
	covers[CENTER_COVER].z = 0;
	covers[CENTER_COVER].angle = 0;
	covers[CENTER_COVER].color = 1;
	covers[CENTER_COVER].pos = CENTER_COVER;

/* set dirlist pointer */
	d = get_dirlist();
	for(i = CENTER_COVER; i >= 0; i--) {
		covers[i].dirlist = d;
		d = d->prev;
	}
	d = get_dirlist();
	for(i = CENTER_COVER; i < NUM_COVERS; i++) {
		covers[i].dirlist = d;
		d = d->next;
	}
/* make textures */
	init_textures();

	for(i = 0; i < NUM_COVERS; i++) {
		covers[i].texture_idx = i;
		load_cover_texture(&covers[i]);
	}
	get_cover_coords();
}

static void shift_covers_left(void) {
int i, tex_id;

	tex_id = covers[0].texture_idx;
	delete_texture(tex_id);

	memmove(&covers[0], &covers[1], sizeof(Cover) * (NUM_COVERS-1));

	for(i = 0; i < NUM_COVERS; i++)
		covers[i].pos = i;

	covers[NUM_COVERS-1].texture_idx = tex_id;
	covers[NUM_COVERS-1].dirlist = covers[NUM_COVERS-1].dirlist->next;

	create_texture(tex_id);
	load_cover_texture(&covers[NUM_COVERS-1]);
}

static void shift_covers_right(void) {
int i, tex_id;

	tex_id = covers[NUM_COVERS-1].texture_idx;
	delete_texture(tex_id);

	memmove(&covers[1], &covers[0], sizeof(Cover) * (NUM_COVERS-1));

	for(i = 0; i < NUM_COVERS; i++)
		covers[i].pos = i;

	covers[0].texture_idx = tex_id;
	covers[0].dirlist = covers[0].dirlist->prev;

	create_texture(tex_id);
	load_cover_texture(&covers[0]);
}

void move_cover_left(Cover *c) {
float xpos, step_x, step_z, step_angle, target_angle, step_color, speed;
int i, anim_done;

	if (!moving)
		return;

	if (SDK_ticks() - ticks_moving > SPEEDUP_DELAY)
		speed = FLIP_SPEED_FAST;
	else
		speed = FLIP_SPEED;

	step_z = 0.0f;
	step_angle = 0.0f;
	target_angle = 0.0f;

	if (c->pos == CENTER_COVER || c->pos == CENTER_COVER+1) {
		step_x = -CENTER_SPACE / speed;
		step_z = -COVER_Z / speed;
		step_angle = COVER_ANGLE / speed;

		if (c->pos == CENTER_COVER) {
			target_angle = COVER_ANGLE;

			c->z -= step_z;
			if (c->z < COVER_Z)
				c->z = COVER_Z;
		} else {
			c->z += step_z;
			if (c->z > 0.0f)
				c->z = 0.0f;
		}
		c->angle += step_angle;
		if (c->angle > target_angle)
			c->angle = target_angle;
	} else
		step_x = -COVER_DISTANCE / speed;

	step_color = 0.1f / speed;
	if (c->pos <= CENTER_COVER)
		step_color = -step_color;

	c->x += step_x;
	c->color += step_color;

/* check new positions */
	anim_done = 0;

/* left side */
	xpos = ARENA_WIDTH * 0.5f - CENTER_SPACE - 5 * COVER_DISTANCE;

	for(i = 2; i <= CENTER_COVER; i++) {
		if (covers[i].x <= xpos) {
			covers[i].x = xpos;
			anim_done++;
		}
		xpos += COVER_DISTANCE;
	}
/* right side */
	xpos = ARENA_WIDTH * 0.5f + CENTER_SPACE + COVER_DISTANCE;

	for(i = CENTER_COVER+2; i < NUM_COVERS-1; i++) {
		if (covers[i].x <= xpos) {
			covers[i].x = xpos;
			anim_done++;
		}
		xpos += COVER_DISTANCE;
	}
/* the cover that just moved into center position */
	xpos = ARENA_WIDTH * 0.5f;

	if (covers[CENTER_COVER+1].x <= xpos) {
		covers[CENTER_COVER+1].x = xpos;
		anim_done++;
	}
	if (anim_done >= NUM_COVERS-3) {
		moving = 0;
		shift_covers_left();
	}
}

void move_cover_right(Cover *c) {
float xpos, step_x, step_z, step_angle, target_angle, step_color, speed;
int i, anim_done;

	if (!moving)
		return;

	if (SDK_ticks() - ticks_moving > SPEEDUP_DELAY)
		speed = FLIP_SPEED_FAST;
	else
		speed = FLIP_SPEED;

	step_z = 0.0f;
	step_angle = 0.0f;
	target_angle = 0.0f;

	if (c->pos == CENTER_COVER || c->pos == CENTER_COVER-1) {
		step_x = CENTER_SPACE / speed;
		step_z = -COVER_Z / speed;
		step_angle = COVER_ANGLE / speed;

		if (c->pos == CENTER_COVER) {
			target_angle = -COVER_ANGLE;

			c->z -= step_z;
			if (c->z < COVER_Z)
				c->z = COVER_Z;
		} else {
			c->z += step_z;
			if (c->z > 0.0f)
				c->z = 0.0f;
		}
		c->angle -= step_angle;
		if (c->angle < target_angle)
			c->angle = target_angle;
	} else
		step_x = COVER_DISTANCE / speed;

	step_color = 0.1f / speed;
	if (c->pos >= CENTER_COVER)
		step_color = -step_color;

	c->x += step_x;
	c->color += step_color;

/* check new positions */
	anim_done = 0;

/* left side */
	xpos = ARENA_WIDTH * 0.5f - CENTER_SPACE - 4 * COVER_DISTANCE;

	for(i = 1; i < CENTER_COVER-1; i++) {
		if (covers[i].x >= xpos) {
			covers[i].x = xpos;
			anim_done++;
		}
		xpos += COVER_DISTANCE;
	}
/* right side */
	xpos = ARENA_WIDTH * 0.5f + CENTER_SPACE + COVER_DISTANCE;

	for(i = CENTER_COVER; i < NUM_COVERS-2; i++) {
		if (covers[i].x >= xpos) {
			covers[i].x = xpos;
			anim_done++;
		}
		xpos += COVER_DISTANCE;
	}
/* the cover that just moved into center position */
	xpos = ARENA_WIDTH * 0.5f;

	if (covers[CENTER_COVER-1].x >= xpos) {
		covers[CENTER_COVER-1].x = xpos;
		anim_done++;
	}
	if (anim_done >= NUM_COVERS-3) {
		moving = 0;
		shift_covers_right();
	}
}

void move_covers_left(void) {
int i;

	for(i = 2; i < NUM_COVERS-1; i++)
		move_cover_left(&covers[i]);
}

void move_covers_right(void) {
int i;

	for(i = 1; i < NUM_COVERS-2; i++)
		move_cover_right(&covers[i]);
}


void draw_cover(Cover *c) {
GLfloat vertex_arr[8] = {
	-COVER_W * 0.5f, COVER_H * 0.5f,
	-COVER_W * 0.5f, -COVER_H * 0.5f,
	COVER_W * 0.5f, COVER_H * 0.5f,
	COVER_W * 0.5f, -COVER_H * 0.5f
};
GLfloat line_arr[8] = {
	-COVER_W * 0.5f, -COVER_H * 0.5f,
	-COVER_W * 0.5f, COVER_H * 0.5f,
	COVER_W * 0.5f, COVER_H * 0.5f,
	COVER_W * 0.5f, -COVER_H * 0.5f
};
GLfloat tex_arr[8] = {
	0, 0,
	0, 1,
	1, 0,
	1, 1,
};
GLfloat tex_reflect[8] = {
	0, 1,
	0, 0,
	1, 1,
	1, 0
};

	glPushMatrix();
	glTranslatef(c->x, c->y, c->z);
	glRotatef(c->angle, 0, 1, 0);

	glColor4f(c->color, c->color, c->color, 1);

	if (c->dirlist->img != NULL && c->dirlist->img[0])
		bind_texture(c->texture_idx);
	else
		bind_texture(TEX_DEFAULT_FOLDER);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_arr);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

/* draw fake reflection */
	glPushMatrix();
	glTranslatef(0, -COVER_H, 0);
/*
	do not use alpha blending; blend makes the reflections blend thru each other,
	which we don't want to happen
*/
	glColor4f(c->color * 0.25f, c->color * 0.25f, c->color * 0.25f, 1);

	glVertexPointer(2, GL_FLOAT, 0, vertex_arr);
	glTexCoordPointer(2, GL_FLOAT, 0, tex_reflect);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glPopMatrix();

/*
	draw border around cover
	for some strange reason, OpenGL rotates the values in the vertex_arr around,
	so we can not reuse it here; I use line_arr instead
*/
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(c->color, c->color, c->color, 1);
	glLineWidth(1.0f);

	glVertexPointer(2, GL_FLOAT, 0, line_arr);
	glDrawArrays(GL_LINE_LOOP, 0, 4);

	glPopMatrix();
}

/*
	draw album title text in orthogonal mode
*/
void draw_title(void) {
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

#ifdef OPENGLES
	glOrthof(0, screen_width, screen_height, 0, -1, 10);
#else
	glOrtho(0, screen_width, screen_height, 0, -1, 10);
#endif

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor4f(1, 1, 1, 1);
	glPrint((screen_width - strlen(covers[CENTER_COVER].dirlist->name) * FONT_W) / 2, screen_height - FONT_H * 2, "%s", covers[CENTER_COVER].dirlist->name);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void draw_covers(void) {
int i;

	glDisable(GL_DEPTH_TEST);
/*
	draw in this order; without depth buffer
	this is mainly because the covers on the far left and right should be in the back
*/
	for(i = 0; i < CENTER_COVER-1; i++)
		draw_cover(&covers[i]);

	for(i = NUM_COVERS-1; i > CENTER_COVER+1; i--)
		draw_cover(&covers[i]);

/*
	enable depth buffer; otherwise the flipping covers might intersect unnaturally and look ugly
*/
	glEnable(GL_DEPTH_TEST);

	draw_cover(&covers[CENTER_COVER-1]);
	draw_cover(&covers[CENTER_COVER]);
	draw_cover(&covers[CENTER_COVER+1]);

	draw_title();
}

/* EOB */
