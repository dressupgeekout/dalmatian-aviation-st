/*
 * DALMATIAN AVIATION ST
 * dalmatianlib.h
 */

#ifndef DALMATIANLIB_H
#define DALMATIANLIB_H

#include <stdint.h>
#include <stdio.h>

#define DALMATIAN_VERSION "v0.0.0a"

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

/* Raster operation options that aren't macro'd already */
#define VR_MODE_REPLACE 1

/*
 * Global game status.
 */
enum AppStatus {
	APP_STATUS_OK,
	APP_STATUS_WANT_QUIT
};
typedef enum AppStatus AppStatus;

/*
 * The possible player-choices from the title screen.
 */
enum TitleScreenChoice {
	TITLE_SCREEN_UNDEFINED,
	TITLE_SCREEN_WANT_PLAY,
	TITLE_SCREEN_WANT_QUIT
};
typedef enum TitleScreenChoice TitleScreenChoice;

/*
 * The possible player-choices from the default screen.
 */
enum DefaultScreenChoice {
	DEFAULT_SCREEN_UNDEFINED,
	DEFAULT_SCREEN_WANT_QUIT
};
typedef enum DefaultScreenChoice DefaultScreenChoice;


/*
 * The currently loaded script, and some metadata.
 */
struct Script {
	char title[16];
	char line1[80];
	char line2[80];
	int8_t beat_index;
};
typedef struct Script Script;

/*
 * The whole game
 */
struct Game {
	int16_t workstation;
	int16_t max_x;
	int16_t max_y;
	int16_t old_x;
	int16_t old_y;
	int16_t money;
	char debug_lines[4][128];
	AppStatus status;
	Script *script;
};
typedef struct Game Game;

void StopClipping(void);
void Blackout(void);
void Whiteout(void);

uint8_t AwaitScancode(void);
void BlitBitmap(const char *path, int16_t x, int16_t y, int16_t w, int16_t h);

void InitGame(Game *game);
void UpdateFunds(const Game *game);

void CharacterSay(const Script *script);
Script *LoadScript(void);
void CloseScript(Script *script);
void NextBeat(Script *script);

/* GLOBAL VARIABLES */
extern Game GAME;

#endif /* DALMATIANLIB_H */
