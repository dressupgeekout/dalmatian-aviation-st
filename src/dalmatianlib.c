/*
 * dalmatianlib.c
 */

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem.h>

#include "dalmatianlib.h"

/*
 * Blanks out the entire screen. Requires that MAX_X and MAX_Y already be
 * defined.
 */
void
Blackout(void)
{
	vsf_color(WORKSTATION, G_BLACK);
	vsf_interior(WORKSTATION, FIS_SOLID); 
	int16_t args[] = {0, 0, MAX_X, MAX_Y};
	v_bar(WORKSTATION, args);
}


/*
 * Refer to Blackout()
 */
void
Whiteout(void)
{
	vsf_color(WORKSTATION, G_WHITE);
	vsf_interior(WORKSTATION, FIS_SOLID);
	int16_t args[] = {0, 0, MAX_X, MAX_Y};
	v_bar(WORKSTATION, args);
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
 * The Script should have already been advanced to the desired beat.
 */
void
CharacterSay(struct Script *script)
{
	static char buf1[80]; 
	static char buf2[80]; 

	const int16_t x = 6;
	const int16_t y1 = MAX_Y - 48;
	const int16_t y2 = MAX_Y - 24;

	/* Clear out both dialogue-lines */
	memset(buf1, ' ', sizeof(buf1));
	memset(buf2, ' ', sizeof(buf2));
	v_gtext(WORKSTATION, x, y1, buf1);
	v_gtext(WORKSTATION, x, y2, buf2);

	/* Now fill them with the actual dialogue */
	snprintf(buf1, sizeof(buf1), "%s", script->line1);
	snprintf(buf2, sizeof(buf2), "%s", script->line2);
	v_gtext(WORKSTATION, x, y1, buf1);
	v_gtext(WORKSTATION, x, y2, buf2);
}


/*
 * Loads a script from disk.
 */
struct Script *
LoadScript(const char *name)
{
	struct Script *script = malloc(sizeof(struct Script));

	snprintf(script->filename, sizeof(script->filename), "%s", name);
	snprintf(script->line1, sizeof(script->line1), "%s", "");
	snprintf(script->line2, sizeof(script->line1), "%s", "");

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/%s", SCRIPT_DIR, name);
	FILE *fp = fopen(path, "r");
	script->fp = fp;

	return script;
}


/*
 * Closes the script file descriptor, and frees memory.
 */
void
CloseScript(struct Script *script)
{
	fclose(script->fp);
	free(script);
}


/*
 * Loads the next line of dialogue into the Script.
 */
void
NextBeat(struct Script *script)
{
	if (!fgets(script->line1, sizeof(script->line1), script->fp)) {
		// assume EOF for now
	}
	if (!fgets(script->line2, sizeof(script->line2), script->fp)) {
		// asume EOF for now
	}
}
