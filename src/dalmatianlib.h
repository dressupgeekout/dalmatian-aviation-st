/*
 * dalmatianlib.h
 */

#ifndef DALMATIANLIB_H
#define DALMATIANLIB_H

#include <stdint.h>
#include <stdio.h>

#define DALMATIAN_VERSION "0.0.0a"
#define SCRIPT_DIR "script"

/* Forward declarations */
enum AppStatus;

/* GLOBAL VARIABLES */
extern int16_t WORKSTATION;
extern int16_t MAX_X;
extern int16_t MAX_Y;
extern char DEBUG_LINES[4][128];
extern enum AppStatus APP_STATUS;
extern struct Script *SCRIPT;

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

void Blackout(void);
void Whiteout(void);

uint8_t AwaitScancode(void);
void CharacterSay(struct Script *script);
struct Script *LoadScript(const char *name);
void CloseScript(struct Script *script);
void NextBeat(struct Script *script);

#endif /* DALMATIANLIB_H */
