/*
 * DALMATIAN AVIATION ST
 * dalmatianlib.c
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem.h>

#include "dalmatianlib.h"
#include "janet.h"
#include "yb.h"

/* Import the CHARACTER_MAP and DIALOGUE_LINES global vars: */
#include "script.h"

/*
 * XXX this SHOULD come from libcmini, I reckon
 *
 * prototype is in
 * /opt/cross-mint/m68k-atari-mint/sys-root/usr/include/stdio.h
 *
 * Maybe I need a newer cross-gcc?
 */
int
__flshfp __P ((FILE *__stream, int __c))
{
	return 1; /* XXX */
}


/*
 * Set the clip-area to the entire screen, i.e, DON'T limit where the
 * screen-draws happen.
 */
void StopClipping(const Game *game)
{
	int16_t clip_rect[] = {0, 0, game->max_x, game->max_y};
	vs_clip(game->workstation, 1, clip_rect);
}


/*
 * Blanks out the entire screen. Requires that MAX_X and MAX_Y already be
 * defined.
 */
void
Blackout(const Game *game)
{
	StopClipping(game);
	vsf_color(game->workstation, G_BLACK);
	vsf_interior(game->workstation, FIS_SOLID); 
	int16_t args[] = {0, 0, game->max_x, game->max_y};
	v_bar(game->workstation, args);
}


/*
 * Refer to Blackout()
 */
void
Whiteout(const Game *game)
{
	StopClipping(game);
	vsf_color(game->workstation, G_WHITE);
	vsf_interior(game->workstation, FIS_SOLID);
	int16_t args[] = {0, 0, game->max_x, game->max_y};
	v_bar(game->workstation, args);
}



/*
 * Waits for a keypress and returns only the scancode (not the ASCII part)
 */
uint8_t
AwaitScancode(void)
{
	return (uint8_t)((evnt_keybd() & 0xff00) >> 8);
}


/*
 * Blits the entire picture somewhere
 *
 * XXX a different function should be used to blit a "quad"
 */
void
BlitYB(const Game *game, const char *path, int16_t x, int16_t y)
{
	YBStatus status;
	YBFile *yb = YBFile_Open(path, &status);

	if (!yb) {
		switch (status) {
		case YB_EBADMAGIC:
			(void)form_alert(1, FA_ERROR "[BlitYB: YB_EBADMAGIC][OK]");
			break;
		case YB_ENOEXIST:
			(void)form_alert(1, FA_ERROR "[BlitYB: YB_ENOEXIST][OK]");
			break;
		case YB_ENOMEM:
			(void)form_alert(1, FA_ERROR "[BlitYB: YB_ENOMEM][OK]");
			break;
		case YB_EUNKNOWN:
			(void)form_alert(1, FA_ERROR "[BlitYB: YB_EUNKNOWN][OK]");
			break;
		default:
			; /* OK */
		}
		return;
	}

	MFDB src;
	bzero(&src, sizeof(src));
	src.fd_addr = yb->data;
	src.fd_w = (int16_t)yb->width;
	src.fd_h = (int16_t)yb->height;
	src.fd_wdwidth = (int16_t)(yb->width/16);
	src.fd_stand = 1;
	src.fd_nplanes = 1;

	MFDB dest;
	bzero(&dest, sizeof(dest));

	int16_t rects[] = {0, 0, yb->width, yb->height, x, y, x+yb->width, y+yb->height};
	int16_t clip_rect[] = {x, y, x+yb->width, y+yb->height};
	int16_t color_index[] = {0, 1};
	vs_clip(game->workstation, 1, clip_rect);
	vrt_cpyfm(game->workstation, VR_MODE_REPLACE, rects, &src, &dest, color_index);
	StopClipping(game);

	YBFile_Close(yb);
}


/*
 * Initializes the Game struct
 */
Game *
InitGame(void)
{
	Game *game = malloc(sizeof(Game));
	bzero(game, sizeof(Game));
	(void)janet_init();
	game->J = janet_core_env(NULL);
	game->money = 1000;
	game->script = NULL;
	for (int i = 0; i < 4; i++) {
		snprintf(game->debug_lines[i], sizeof(game->debug_lines[i]), "%s", "");
	}
	return game;
}


void
DeleteGame(Game *game)
{
	CloseScript(game->script);
	janet_deinit();
	free(game);
}


/*
 * Displays the amount of money the player has.
 */
void UpdateFunds(const Game *game)
{
	static char buf[16];
	const int16_t x = game->max_x - 100;
	const int16_t y = 12;

	memset(buf, ' ', sizeof(buf));
	buf[15] = '\0';
	v_gtext(game->workstation, x, y, buf);

	snprintf(buf, sizeof(buf), "FUNDS: $%d", game->money);
	v_gtext(game->workstation, x, y, buf);
}


/*
 * The Script should have already been advanced to the desired beat.
 */
void
CharacterSay(const Game *game)
{
	static char buf1[80]; 
	static char buf2[80]; 

	const int16_t x = 6;
	const int16_t y1 = game->max_y - 48;
	const int16_t y2 = game->max_y - 24;

	/* Clear out both dialogue-lines */
	memset(buf1, ' ', sizeof(buf1));
	memset(buf2, ' ', sizeof(buf2));
	buf1[79] = '\0';
	buf2[79] = '\0';
	v_gtext(game->workstation, x, y1, buf1);
	v_gtext(game->workstation, x, y2, buf2);

	/* Now fill them with the actual dialogue */
	snprintf(buf1, sizeof(buf1), "%s", game->script->line1);
	snprintf(buf2, sizeof(buf2), "%s", game->script->line2);
	v_gtext(game->workstation, x, y1, buf1);
	v_gtext(game->workstation, x, y2, buf2);
}


/*
 * Loads a script from disk.
 */
Script *
LoadScript(void)
{
	Script *script = malloc(sizeof(Script));
	bzero(script, sizeof(*script));
	script->beat_index = -1; /* Such that the subsequent NextBeat() goes to 0 */
	return script;
}


/*
 * Closes the script file descriptor, and frees memory.
 */
void
CloseScript(Script *script)
{
	free(script);
	script = NULL;
}


/*
 * Loads the next line of dialogue into the Script.
 */
void
NextBeat(Script *script)
{
	if (script->beat_index >= (NUM_BEATS-1)) return;
	script->beat_index++;
	snprintf(
		script->line1, sizeof(script->line1),
		"%s: %s",
		CHARACTER_MAP[DIALOGUE_LINES[script->beat_index].character],
		DIALOGUE_LINES[script->beat_index].line);
}
