/*
 * dalmatianlib.h
 */

#ifndef DALMATIANLIB_H
#define DALMATIANLIB_H

#include <stdint.h>

/* GLOBAL VARIABLES */
extern int16_t WORKSTATION;
extern int16_t MAX_X;
extern int16_t MAX_Y;

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

#define CHAR_WHITNEY "WHITNEY"
#define CHAR_KRISSANY "KRISSANY"
#define CHAR_INVALID "__INVALID__"

/*
 * The possible player-choices from the title screen.
 */
enum TitleScreenChoice {
	TITLE_SCREEN_UNDEFINED,
	TITLE_SCREEN_WANT_PLAY,
	TITLE_SCREEN_WANT_QUIT,
};

struct Dialogue {
	char character[16];
	char line[192];
};

uint8_t AwaitScancode(void);
void CharacterSay(struct Dialogue *dialogue);

#endif /* DALMATIANLIB_H */
