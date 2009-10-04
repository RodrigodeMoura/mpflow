/*
	text.h	WJ108
*/

#ifndef TEXT_H_WJ108
#define TEXT_H_WJ108	1

#include "main.h"

#include "SDK.h"

#define FONT_LINING		2			/* black line around each character */
#define FONT_SPACING	0

/*
	I need at least 16 textboxes for printing the hiscore table ...
#define NUM_TEXT_IDS	((ARENA_HEIGHT / (FONT_H+FONT_LINING)) + 2)
*/
#define NUM_TEXT_IDS	16

#define TEXT_TILE_W		1024		/* next_power_of_two(ARENA_WIDTH) */
#define TEXT_TILE_H		16			/* next_power_of_two(FONT_H+FONT_LINING) */
#define TEXT_TILE_BPP	4

/* text string color codes */
#define STR_BLACK		"\xf0"
#define CHAR_BLACK		0xf0
#define STR_RED			"\xf1"
#define STR_GREEN		"\xf2"
#define STR_YELLOW		"\xf3"
#define STR_BLUE		"\xf4"
#define STR_MAGENTA		"\xf5"
#define STR_CYAN		"\xf6"
#define STR_WHITE		"\xf7"
#define CHAR_WHITE		0xf7

/* text rendering flags */
#define TEXT_MONOSPACE	1

typedef enum {
	BLACK = 0,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
} TextColor;

typedef struct {
	unsigned char r, g, b, a;
} Color4;

typedef struct TextBox_tag TextBox;

struct TextBox_tag {
	float x, y;
	int w, h, flags;

	GLuint tex_id;

	TextColor color;
	char *str;

	TextBox *prev, *next;
};

void init_text(void);
void reinit_text(void);
TextBox *new_TextBox(void);
void destroy_TextBox(TextBox *);
TextBox *render_text(float, float, char *, TextColor);
TextBox *center_text(float, float, char *, TextColor);
TextBox *render_text_monospace(float, float, char *, TextColor);
TextBox *center_text_monospace(float, float, char *, TextColor);
void draw_text(void);
void clear_text(void);
void setpos_text(TextBox *, float, float);

#endif	/* TEXT_H_WJ108 */

/* EOB */
