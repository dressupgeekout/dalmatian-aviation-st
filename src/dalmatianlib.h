/*
 * DALMATIAN AVIATION ST
 * dalmatianlib.h
 */

#ifndef DALMATIANLIB_H
#define DALMATIANLIB_H

#include <stdint.h>
#include <stdio.h>

#define DALMATIAN_VERSION "0.0.0a"
#define SCRIPT_DIR "script"

/* Keyboard scancodes that aren't macro'd already for some reason */
#define K_F1 59
#define K_F2 60
#define K_F3 61
#define K_F4 62
#define K_F5 63
#define K_F6 64
#define K_F7 65
#define K_F8 66
#define K_F9 67
#define K_F10 68
#define K_SPACE 57

#define CHAR_KRISSANY "KRISSANY"
#define CHAR_WHITNEY "WHITNEY"
#define CHAR_INVALID "__INVALID__"


/*
 * Global game status.
 */
enum AppStatus {
	APP_STATUS_OK,
	APP_STATUS_WANT_QUIT
};


/*
 * The possible player-choices from the title screen.
 */
enum TitleScreenChoice {
	TITLE_SCREEN_UNDEFINED,
	TITLE_SCREEN_WANT_PLAY,
	TITLE_SCREEN_WANT_QUIT
};


/*
 * The possible player-choices from the default screen.
 */
enum DefaultScreenChoice {
	DEFAULT_SCREEN_UNDEFINED,
	DEFAULT_SCREEN_WANT_QUIT
};


/*
 * The currently loaded script, and some metadata.
 */
struct Script {
	char filename[12];
	FILE *fp;
	char line1[80];
	char line2[80];
};

/*
 * The whole game
 */
struct Game {
	int16_t workstation;
	int16_t max_x;
	int16_t max_y;
	char debug_lines[5][128];
	enum AppStatus status;
	struct Script *script;
};

void Blackout(void);
void Whiteout(void);

uint8_t AwaitScancode(void);

void CharacterSay(const struct Script *script);
struct Script *LoadScript(const char *name);
void CloseScript(struct Script *script);
void NextBeat(struct Script *script);

/* GLOBAL VARIABLES */
extern struct Game GAME;

#endif /* DALMATIANLIB_H */
