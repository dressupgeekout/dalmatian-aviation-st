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
int16_t MAX_X;
int16_t MAX_Y;
int16_t WORKSTATION;
char DEBUG_LINES[4][128];
enum AppStatus APP_STATUS;
struct Script *SCRIPT;

/* Function prototypes */
static bool enforce_hires(void);
static void dots_intro(void);
static enum TitleScreenChoice DoTitleScreen(void);
static enum DefaultScreenChoice DoDefaultScreen(void);

static void handle_keyboard(EVMULT_OUT *events);
static void handle_mouse(EVMULT_OUT *events);

/* ********** */

/*
 * Returns true if the user's monitor has the required specs, false otherwise.
 */
static bool
enforce_hires(void)
{
	const int16_t EXPECTED_MAXX = 640;
	const int16_t EXPECTED_MAXY = 400;

	if (MAX_X+1 != EXPECTED_MAXX || MAX_Y+1 != EXPECTED_MAXY) {
		char buf[256];
		snprintf(
			buf, sizeof(buf),
			FA_ERROR "[Sorry, Dalmatian Aviation|requires monochrome/hi-res mode.|%dx%d != %dx%d][OK]",
			MAX_X+1, MAX_Y+1, EXPECTED_MAXX, EXPECTED_MAXY);
		(void)form_alert(1, buf);
		return false;
	}

	return true;
}

/*
 * Covers the screen in pseudo-random black dots (animation)
 *
 * XXX all this data prolly belongs in its own header file
 */
static void
dots_intro(void)
{
#define DOT_FRAMES 48
	int16_t xs[DOT_FRAMES] = {
		311, 38, 441, 269, 196, 379, 209, 545, 554, 302, 416, 96, 473, 320, 310,
		598, 509, 369, 170, 543, 394, 611, 164, 573, 29, 620, 557, 608, 291,
		587, 205, 47, 446, 24, 406, 356, 553, 102, 306, 118, 77, 406, 470, 146,
		171, 369, 127, 379,
	};

	int16_t ys[DOT_FRAMES] = {
		365, 224, 152, 203, 121, 272, 35, 90, 389, 329, 311, 227, 106, 44, 287,
		107, 399, 207, 242, 260, 378, 158, 338, 170, 9, 275, 309, 261, 46, 56,
		195, 366, 291, 27, 309, 164, 384, 199, 396, 166, 293, 220, 378, 56, 386,
		107, 282, 167,
	};

	int16_t dot_rs[DOT_FRAMES] = {
		19, 36, 2, 22, 27, 15, 16, 20, 12, 12, 23, 27, 13, 3, 34, 9, 34, 38, 18,
		32, 26, 31, 6, 22, 31, 22, 26, 29, 37, 5, 3, 39, 4, 5, 3, 31, 36, 27,
		22, 3, 15, 14, 15, 4, 4, 7, 7, 6,
	};

	int i;
	vsf_color(WORKSTATION, G_BLACK);
	vsf_interior(WORKSTATION, FIS_SOLID);

	wind_update(BEG_UPDATE);
	for (i = 0; i < DOT_FRAMES; i++) {
		v_circle(WORKSTATION, xs[i], ys[i], dot_rs[i]);
		(void)evnt_timer(10); /* Wait 10 ms */
	}
	wind_update(END_UPDATE);
#undef DOT_FRAMES

	Blackout();
	(void)evnt_timer(1000); /* Wait 1 sec */
}


/*
 * The player can choose to play the game, or to quit.
 */
static enum TitleScreenChoice
DoTitleScreen(void)
{
	Whiteout();
	v_gtext(WORKSTATION, 100, 100, "DALMATIAN AVIATION ST");
	v_gtext(WORKSTATION, 100, 124, "by Dressupgeekout");
	v_gtext(WORKSTATION, 100, 200, " F1 - PLAY");
	v_gtext(WORKSTATION, 100, 224, "F10 - QUIT");

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
handle_keyboard(EVMULT_OUT *events)
{
	snprintf(DEBUG_LINES[1], sizeof(DEBUG_LINES[1]), "kreturn=%02x", events->emo_kreturn);
	v_gtext(WORKSTATION, 0, 12+24, DEBUG_LINES[1]);

	uint8_t scancode = (events->emo_kreturn & 0xff00) >> 8;

	switch (scancode) {
	case K_SPACE:
		NextBeat(SCRIPT);
		CharacterSay(SCRIPT);
		break;
	case K_F10:
		APP_STATUS = APP_STATUS_WANT_QUIT;
		break;
	default:
		; /* OK */
	}
}


static void
handle_mouse(EVMULT_OUT *events)
{
	snprintf(DEBUG_LINES[2], sizeof(DEBUG_LINES[2]),
		"mx=%d my=%d mbutton=0x%02x    ",
		events->emo_mouse.p_x, events->emo_mouse.p_y, events->emo_mbutton);
	v_gtext(WORKSTATION, 0, 24+24+6, DEBUG_LINES[2]);
}


static enum DefaultScreenChoice
DoDefaultScreen(void)
{
	Whiteout();

	SCRIPT = LoadScript("TEST01.TXT");
	NextBeat(SCRIPT);
	CharacterSay(SCRIPT);

	EVMULT_IN event_in;
	bzero(&event_in, sizeof(event_in));
	event_in.emi_flags = MU_KEYBD | MU_BUTTON | MU_M1; /* which events to pay att'n to */
	event_in.emi_bclicks = 1; /* max clicks to consider */
	event_in.emi_bmask = LEFT_BUTTON | RIGHT_BUTTON; /* which mouse-buttons to consider */
	event_in.emi_bstate = 0xf; /* only consider mouse-downs */
	event_in.emi_m1leave = MO_ENTER; /* "enter the whole screen" means we see every mouse-movement */
	GRECT rect = {0, 0, MAX_X, MAX_Y};
	event_in.emi_m1 = rect;

	EVMULT_OUT event_out;

	bool done = false;

	while (!done) {
		evnt_multi_fast(&event_in, NULL, &event_out);

		snprintf(DEBUG_LINES[0], sizeof(DEBUG_LINES[0]), "emo_events=0x%04x    ", event_out.emo_events);
		v_gtext(WORKSTATION, 0, 12, DEBUG_LINES[0]);

		if (event_out.emo_events & MU_KEYBD) {
			handle_keyboard(&event_out);
		}

		if (event_out.emo_events & (MU_BUTTON | MU_M1)) {
			handle_mouse(&event_out);
		}

		/* Analyze the results of having handled the events. */
		if (APP_STATUS == APP_STATUS_WANT_QUIT) {
			CloseScript(SCRIPT);
			done = true;
		}
	}

	return DEFAULT_SCREEN_WANT_QUIT;
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
	work_in[0] = 1; /* Want a workstation @ current resolution */
	v_opnvwk(work_in, &WORKSTATION, work_out);

	if (!WORKSTATION) {
		(void)appl_exit(); /* appl_exit() itself might fail */
		return EXIT_FAILURE;
	}

	MAX_X = work_out[0];
	MAX_Y = work_out[1];

	/* Require high-resolution monitor. */
	if (!enforce_hires()) {
		v_clsvwk(WORKSTATION);
		(void)appl_exit();
		return EXIT_FAILURE;
	}

	/* All text will be black henceforth */
	vst_color(WORKSTATION, G_BLACK);

	APP_STATUS = APP_STATUS_OK;

	bool want_intro = true; /* XXX command-line option */
	enum TitleScreenChoice choice;

	if (want_intro) {
		dots_intro();
		choice = DoTitleScreen();
	} else {
		choice = TITLE_SCREEN_WANT_PLAY;
	}

	switch (choice) {
		case TITLE_SCREEN_WANT_PLAY:
			DoDefaultScreen();
			break;
		case TITLE_SCREEN_WANT_QUIT:
			; /* ignore, just quit */
			break;
		default:
			; /* NOTREACHED */
	}

	/* Quit */ 
	v_clsvwk(WORKSTATION);
	(void)appl_exit();
	return EXIT_SUCCESS;
}
