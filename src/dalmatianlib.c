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
#include "daljan.h"
#include "janet.h"
#include "yb.h"

/*
 * XXX this SHOULD come from libcmini, I reckon
 * Maybe I need a newer cross-gcc?
 *
 * prototype is in:
 * /opt/cross-mint/m68k-atari-mint/sys-root/usr/include/stdio.h
 *
 * "This performs actual output when necessary, flushing STREAM's buffer and
 * optionally writing another character."
 */
int
__flshfp __P ((FILE *__stream, int __c))
{
	fflush(__stream);
	return 0;
}


/*
 * Set the clip-area to the entire screen, i.e, DON'T limit where the
 * screen-draws happen.
 */
void StopClipping(const Game *game)
{
	int16_t clip_rect[] = {0, 0, game->maxpt.p_x, game->maxpt.p_y};
	vs_clip(game->workstation, 1, clip_rect);
}


/*
 * Blanks out the entire screen. Requires that Game->maxpt already be
 * computed.
 */
void
Blackout(const Game *game)
{
	StopClipping(game);
	vsf_color(game->workstation, G_BLACK);
	vsf_interior(game->workstation, FIS_SOLID); 
	int16_t args[] = {0, 0, game->maxpt.p_x, game->maxpt.p_y};
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
	int16_t args[] = {0, 0, game->maxpt.p_x, game->maxpt.p_y};
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
 * Allocates and initializes the Game struct. Returns NULL in case of failure.
 * You need to free the result with DeleteGame().
 */
Game *
InitGame(void)
{
	Game *game = calloc(1, sizeof(Game));

	if (!game) {
		return NULL;
	}

	game->mouse_is_down = false;
	game->beat_index = -1;

	/* Initialize Janet interpreter */
	(void)janet_init();
	game->J = janet_core_env(NULL);

	/* Janet: load Dalmatian library */
	static const JanetReg functions[] = {
		{"form-alert", dj_form_alert, NULL},
		{NULL, NULL, NULL}
	};
	janet_cfuns(game->J, NULL, functions);

	/* Initialize debug-lines */
	for (int i = 0; i < GAME_DEBUGLINE_COUNT; i++) {
		snprintf(game->debug_lines[i], GAME_DEBUGLINE_LEN, "%s", "");
	}

	/* Misc */
	game->money = 1000;

	return game;
}


void
DeleteGame(Game *game)
{
	janet_deinit();
	free(game);
}


/*
 * Displays the amount of money the player has.
 */
void UpdateFunds(const Game *game)
{
	static char buf[16];
	const int16_t x = game->maxpt.p_x - 100;
	const int16_t y = 12;

	memset(buf, ' ', sizeof(buf));
	buf[15] = '\0';
	v_gtext(game->workstation, x, y, buf);

	snprintf(buf, sizeof(buf), "FUNDS: $%d", game->money);
	v_gtext(game->workstation, x, y, buf);
}


/*
 * Advance the beat-index and populate the dialogue-lines with the resultant
 * dialogue.
 */
void
NextBeat(Game *game)
{
	if (game->beat_index < (janet_tuple_length(game->dialogue_tree)-1)) {
		game->beat_index++;
		DoCurrentBeat(game);
	}
}


/*
 * We should have already been advanced to the desired beat.
 */
void
DoCurrentBeat(Game *game)
{
	static char buf1[80]; 
	static char buf2[80]; 
	static char errbuf[80];

	const int16_t x = 6;
	const int16_t y1 = game->maxpt.p_y - 48;
	const int16_t y2 = game->maxpt.p_y - 24;

	/* Clear out both dialogue-lines */
	memset(buf1, ' ', sizeof(buf1));
	memset(buf2, ' ', sizeof(buf2));
	buf1[79] = '\0';
	buf2[79] = '\0';
	v_gtext(game->workstation, x, y1, buf1);
	v_gtext(game->workstation, x, y2, buf2);

	/* LOOTS of type-checking */
	if (!janet_checktype(game->dialogue_tree[game->beat_index], JANET_FUNCTION)) {
		snprintf(errbuf, sizeof(errbuf), "%s[beat %d is not a fn][OK]", FA_ERROR, game->beat_index);
		return (void)form_alert(1, errbuf);
	}

	JanetFunction *beatproc = janet_unwrap_function(game->dialogue_tree[game->beat_index]);
	Janet beat_w;
	JanetSignal signal = janet_pcall(beatproc, 0, NULL, &beat_w, NULL);

	if (signal != JANET_SIGNAL_OK) {
		snprintf(errbuf, sizeof(errbuf), "%s[beat %d fn is bad|signal=%d][OK]", FA_ERROR, game->beat_index, signal);
		return (void)form_alert(1, errbuf);
	}

	if (!janet_checktype(beat_w, JANET_STRUCT)) {
		snprintf(errbuf, sizeof(errbuf), "%s[beat %d didn't return a struct][OK]", FA_ERROR, game->beat_index);
		return (void)form_alert(1, errbuf);
	}

	JanetStruct beat = janet_unwrap_struct(beat_w);
	Janet speaker = janet_struct_get(beat, janet_ckeywordv("speaker"));
	Janet lines_w = janet_struct_get(beat, janet_ckeywordv("lines"));

	if (!janet_checktype(speaker, JANET_STRING)) {
		snprintf(errbuf, sizeof(errbuf), "%s[beat %d|'speaker' is not a string][OK]", FA_ERROR, game->beat_index);
		return (void)form_alert(1, errbuf);
	}

	if (!janet_checktype(lines_w, JANET_TUPLE)) {
		snprintf(errbuf, sizeof(errbuf), "%s[beat %d|'lines' is not a tuple][OK]", FA_ERROR, game->beat_index);
		return (void)form_alert(1, errbuf);
	}

	const uint8_t EXPECTED_LENGTH = 2;
	const Janet *lines = janet_unwrap_tuple(lines_w);
	int16_t lines_length = janet_tuple_length(lines);

	if (lines_length != EXPECTED_LENGTH) {
		snprintf(errbuf, sizeof(errbuf),
			"%s[beat %d|wrong lines length|%d != %d][OK]", FA_ERROR, game->beat_index, lines_length, EXPECTED_LENGTH);
		return (void)form_alert(1, errbuf);
	}

	for (int i = 0; i < 2; i++) {
		if (!janet_checktype(lines[i], JANET_STRING)) {
			snprintf(errbuf, sizeof(errbuf),
				"%s[beat %d|line %d is not a string|%d != %d][OK]", FA_ERROR, game->beat_index, i, JANET_STRING, janet_type(lines[i]));
			return (void)form_alert(1, errbuf);
		}
	}

	/* If there are effects with this beat, then resolve them now. */
	Janet effects_w = janet_struct_get(beat, janet_ckeywordv("effects"));

	if (janet_checktype(effects_w, JANET_STRUCT)) {
		JanetStruct effects = janet_unwrap_struct(effects_w);
		Janet add_money_effect = janet_struct_get(effects, janet_ckeywordv("add-money"));
		if (janet_checktype(add_money_effect, JANET_NUMBER)) {
			game->money += janet_unwrap_number(add_money_effect);
			UpdateFunds(game);
		}
	}

	/* Finally! */
	snprintf(buf1, sizeof(buf1), "%s: %s", janet_unwrap_string(speaker), janet_unwrap_string(lines[0]));
	snprintf(buf2, sizeof(buf2), "%s", janet_unwrap_string(lines[1]));
	v_gtext(game->workstation, x, y1, buf1);
	v_gtext(game->workstation, x, y2, buf2);
}


/*
 * Loads a script from disk.
 */
void
LoadScript(Game *game, const char *path)
{
	/* Slurp the file */
	FILE *fp = fopen(path, "r");
	if (!fp) {
		(void)form_alert(1, FA_ERROR "[fopen() crash][OK]");
		return;
	}

	fseek(fp, 0L, SEEK_END);
	size_t script_length = ftell(fp) + 1;
	rewind(fp);
	char *script = calloc(1, script_length);
	if (!script) {
		(void)form_alert(1, FA_ERROR "[calloc() crash][OK]");
		return;
	}
	fread(script, 1, script_length, fp);
	fclose(fp);

	/* Read the script! */
	Janet script_main;
	int errflags = janet_dobytes(game->J, (const uint8_t *)script, script_length, path, &script_main);

	if (errflags & 0x01) {
		(void)form_alert(1, FA_ERROR "[JANET_SIGNAL not OK][OK]");
	}

	if (errflags & 0x02) {
		(void)form_alert(1, FA_ERROR "[compile error][OK]");
	}

	if (errflags & 0x04) {
		(void)form_alert(1, FA_ERROR "[parse error][OK]");
	}

	free(script);
}


void
LoadDialogueScript(Game *game, const char *path)
{
	LoadScript(game, path);

	JanetBinding binding = janet_resolve_ext(game->J, janet_csymbol("__BEATS__"));

	if (binding.type == JANET_BINDING_DEF) {
		if (janet_checktype(binding.value, JANET_TUPLE)) {
			game->dialogue_tree = (Janet *)janet_unwrap_tuple(binding.value);
			char buf[128];
			snprintf(buf, sizeof(buf), "%s[talk has %d beats][OK]", FA_ERROR, janet_tuple_length(game->dialogue_tree));
			(void)form_alert(1, buf);
		} else {
			(void)form_alert(1, FA_ERROR "[__BEATS__ is not an array][OK]");
		}
	} else {
		(void)form_alert(1, FA_ERROR "[script has no def __BEATS__][OK]");
	}
}
