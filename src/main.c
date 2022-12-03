/*
 * DALMATIAN AVIATION ST
 * main.c
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <gem.h>

#include "dalmatianlib.h"

/* ********** */

/* GLOBALS */

#define DOT_FRAMES 48

int16_t MAX_X;
int16_t MAX_Y;
int16_t vworkstation;

static void dots_intro(void);
static void blackout(void);
static void whiteout(void);
static enum TitleScreenChoice title_screen(void);
static void default_screen(void);

/* ********** */

/*
 * Covers the screen in pseudo-random black dots (animation)
 *
 * XXX all this data prolly belongs in its own header file
 */
static void
dots_intro(void)
{
	static int16_t xs[DOT_FRAMES] = {
		311, 38, 441, 269, 196, 379, 209, 545, 554, 302, 416, 96, 473, 320, 310,
		598, 509, 369, 170, 543, 394, 611, 164, 573, 29, 620, 557, 608, 291,
		587, 205, 47, 446, 24, 406, 356, 553, 102, 306, 118, 77, 406, 470, 146,
		171, 369, 127, 379,
	};

	static int16_t ys[DOT_FRAMES] = {
		365, 224, 152, 203, 121, 272, 35, 90, 389, 329, 311, 227, 106, 44, 287,
		107, 399, 207, 242, 260, 378, 158, 338, 170, 9, 275, 309, 261, 46, 56,
		195, 366, 291, 27, 309, 164, 384, 199, 396, 166, 293, 220, 378, 56, 386,
		107, 282, 167,
	};

	static int16_t dot_rs[DOT_FRAMES] = {
		19, 36, 2, 22, 27, 15, 16, 20, 12, 12, 23, 27, 13, 3, 34, 9, 34, 38, 18,
		32, 26, 31, 6, 22, 31, 22, 26, 29, 37, 5, 3, 39, 4, 5, 3, 31, 36, 27,
		22, 3, 15, 14, 15, 4, 4, 7, 7, 6,
	};

	int i;
	vsf_color(vworkstation, G_BLACK);
	vsf_interior(vworkstation, 1); /* Solid fill pattern */

	for (i = 0; i < DOT_FRAMES; i++) {
		v_circle(vworkstation, xs[i], ys[i], dot_rs[i]);
		/* XXX wait for a specific dt ? */
	}

	blackout();
	/* XXX wait for a bit somehow, afterward? */
}


/*
 * Blanks out the entire screen. Requires that MAX_X and MAX_Y already be
 * defined.
 */
static void
blackout(void)
{
	vsf_color(vworkstation, G_BLACK);
	vsf_interior(vworkstation, 1); /* solid fill pattern */
	int16_t args[4] = {0, 0, MAX_X, MAX_Y};
	v_bar(vworkstation, args);
}


/*
 * Refer to blackout()
 */
static void
whiteout(void)
{
	vsf_color(vworkstation, G_WHITE);
	vsf_interior(vworkstation, 1); /* solid fill pattern */
	int16_t args[4] = {0, 0, MAX_X, MAX_Y};
	v_bar(vworkstation, args);
}


/*
 * The player can choose to play the game, or to quit.
 */
static enum TitleScreenChoice
title_screen(void)
{
	whiteout();
	vst_color(vworkstation,	G_BLACK);
	v_gtext(vworkstation, 100, 100, "DALMATIAN AVIATION ST");
	v_gtext(vworkstation, 100, 124, "by Dressupgeekout");
	v_gtext(vworkstation, 100, 200, " F1 - PLAY");
	v_gtext(vworkstation, 100, 224, "F10 - QUIT");

	enum TitleScreenChoice choice = TITLE_SCREEN_UNDEFINED;
	int16_t scancode;

	while (choice == TITLE_SCREEN_UNDEFINED) {
		scancode = AwaitScancode();

		switch (scancode) {
		case K_F1:
			choice = TITLE_SCREEN_WANT_PLAY;
			break;
		case K_F10:
			choice = TITLE_SCREEN_WANT_QUIT;
			break;
		default:
			; /* ignore */
		}
	}

	return choice;
}


static void
default_screen(void)
{
	whiteout();

	static struct Dialogue dialogues[] = {
		{CHAR_WHITNEY, "Hey!"},
		{CHAR_WHITNEY, "Do you hear what's going on over here? Isn't that pretty cool?"},
		{CHAR_KRISSANY, "Yabba dabba doo"},
		{CHAR_WHITNEY, "What?"},
		{CHAR_KRISSANY, "You heard me!"},
		{CHAR_INVALID, ""}
	};

	int i = 0;

	/* XXX strcmp is slow */
	while (strncmp(dialogues[i].character, CHAR_INVALID, sizeof(dialogues[i].character))) {
		CharacterSay(&dialogues[i]);
		(void)AwaitScancode();
		i++;
	}
}


/* ********** */

int
main(void)
{
	/* Init */
	(void)appl_init();

	/* Set up the virtual workstation */
	int16_t work_in[16];
	int16_t work_out[64];
	bzero(work_in, sizeof(work_in));
	bzero(work_out, sizeof(work_out));
	work_in[0] = 1; /* Want a workstation @ current resolution */
	v_opnvwk(work_in, &vworkstation, work_out);

	if (!vworkstation) {
		(void)appl_exit(); /* appl_exit() itself might fail */
		return EXIT_FAILURE;
	}

	MAX_X = work_out[0];
	MAX_Y = work_out[1];

	//dots_intro();
	enum TitleScreenChoice choice = title_screen();

	switch (choice) {
		case TITLE_SCREEN_WANT_PLAY: /* XXX FALLTHROUGH */
			default_screen();
		case TITLE_SCREEN_WANT_QUIT:
			; /* ignore, just quit */
			break;
		default:
			; /* NOTREACHED */
	}

	/* Quit */ 
	v_clsvwk(vworkstation);
	(void)appl_exit();
	return EXIT_SUCCESS;
}
