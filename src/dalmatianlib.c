/*
 * dalmatianlib.c
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <gem.h>

#include "dalmatianlib.h"


/*
 * Waits for a keypress and returns only the scancode (not the ASCII part)
 */
uint8_t
AwaitScancode(void)
{
		return (uint8_t)((evnt_keybd() & 0xff00) >> 8);
}

void
CharacterSay(struct Dialogue *dialogue)
{
	/* XXX bufsiz should be more strategic */
	static char buf[256]; 
	const int16_t x = 12;
	const int16_t y = MAX_Y - 24;

	memset(buf, ' ', sizeof(buf));
	vst_color(vworkstation, G_BLACK);
	v_gtext(vworkstation, x, y, buf);

	snprintf(buf, sizeof(buf), "%s: %s", dialogue->character, dialogue->line);
	v_gtext(vworkstation, x, y, buf);
}
