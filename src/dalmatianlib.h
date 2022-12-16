/*
 * DALMATIAN AVIATION ST
 * dalmatianlib.h
 */

#ifndef DALMATIANLIB_H
#define DALMATIANLIB_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "janet.h"

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
 * The whole game
 */

#define GAME_DEBUGLINE_COUNT 4
#define GAME_DEBUGLINE_LEN 128

struct Game {
	int16_t workstation;
	PXY maxpt;
	PXY mouse_down_spot;
	PXY mouse_up_spot;
	bool mouse_is_down;
	int16_t money;
	char debug_lines[GAME_DEBUGLINE_COUNT][GAME_DEBUGLINE_LEN];
	AppStatus status;
	int16_t beat_index;
	JanetTable *J;
	Janet *dialogue_tree;
};
typedef struct Game Game;

void StopClipping(const Game *game);
void Blackout(const Game *game);
void Whiteout(const Game *game);

uint8_t AwaitScancode(void);
void BlitYB(const Game *game, const char *path, int16_t x, int16_t y);

Game *InitGame(void);
void DeleteGame(Game *game);
void UpdateFunds(const Game *game);

void NextBeat(Game *game);
void CharacterSay(const Game *game);

void LoadScript(Game *game, const char *path);
void LoadDialogueScript(Game *game, const char *path);

#endif /* DALMATIANLIB_H */
