/*
 * DALMATIAN AVIATION ST
 * daljan.c
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gem.h>

#include "janet.h"
#include "daljan.h"


/*
 * (form-alert text)
 */
Janet
dj_form_alert(int32_t argc, Janet *argv)
{
	janet_fixarity(argc, 1);
	const char *text = janet_getcstring(argv, 0);
	int real_size = strlen(FA_ERROR) + strlen(text) + strlen("[OK]") + 4;
	char *buf = malloc(real_size); /* XXX complain if ENOMEM */
	snprintf(buf, real_size, "%s[%s][OK]", FA_ERROR, text);
	(void)form_alert(1, buf);
	free(buf);
	return janet_wrap_nil();
}
