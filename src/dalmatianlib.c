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

/* Import the CHARACTER_MAP and DIALOGUE_LINES global vars: */
#include "script.h"

/*
 * Blanks out the entire screen. Requires that MAX_X and MAX_Y already be
 * defined.
 */
void
Blackout(void)
{
	vsf_color(GAME.workstation, G_BLACK);
	vsf_interior(GAME.workstation, FIS_SOLID); 
	int16_t args[] = {0, 0, GAME.max_x, GAME.max_y};
	v_bar(GAME.workstation, args);
}


/*
 * Refer to Blackout()
 */
void
Whiteout(void)
{
	vsf_color(GAME.workstation, G_WHITE);
	vsf_interior(GAME.workstation, FIS_SOLID);
	int16_t args[] = {0, 0, GAME.max_x, GAME.max_y};
	v_bar(GAME.workstation, args);
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
BlitBitmap(const char *path, int16_t x, int16_t y, int16_t w, int16_t h)
{
	FILE *fp = fopen(path, "rb");
	fseek(fp, 0, SEEK_END);
	uint32_t fsize = ftell(fp);
	int16_t *bitmap = malloc(fsize);
	rewind(fp);
	fread(bitmap, fsize, 1, fp);
	fclose(fp);

	MFDB src;
	bzero(&src, sizeof(src));
	src.fd_addr = bitmap;
	src.fd_w = w;
	src.fd_h = h;
	src.fd_wdwidth = w/16;
	src.fd_stand = 1;
	src.fd_nplanes = 1;

	MFDB dest;
	bzero(&dest, sizeof(dest));

	int16_t rects[] = {0, 0, w, h, x, y, x+w, y+h};
	int16_t color_index[] = {0, 1};
	vrt_cpyfm(GAME.workstation, VR_MODE_REPLACE, rects, &src, &dest, color_index);
	free(bitmap);
}


/*
 * The Script should have already been advanced to the desired beat.
 */
void
CharacterSay(const Script *script)
{
	static char buf1[80]; 
	static char buf2[80]; 

	const int16_t x = 6;
	const int16_t y1 = GAME.max_y - 48;
	const int16_t y2 = GAME.max_y - 24;

	/* Clear out both dialogue-lines */
	memset(buf1, ' ', sizeof(buf1));
	memset(buf2, ' ', sizeof(buf2));
	v_gtext(GAME.workstation, x, y1, buf1);
	v_gtext(GAME.workstation, x, y2, buf2);

	/* Now fill them with the actual dialogue */
	snprintf(buf1, sizeof(buf1), "%s", script->line1);
	snprintf(buf2, sizeof(buf2), "%s", script->line2);
	v_gtext(GAME.workstation, x, y1, buf1);
	v_gtext(GAME.workstation, x, y2, buf2);
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
	script->beat_index++;
	snprintf(
		script->line1, sizeof(script->line1),
		"%s: %s",
		CHARACTER_MAP[DIALOGUE_LINES[script->beat_index].character],
		DIALOGUE_LINES[script->beat_index].line);
}
