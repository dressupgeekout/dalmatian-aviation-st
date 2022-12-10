/*
 * yb.c
 */

#include <sys/stat.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset()

#include "yb.h"

/* GLOBAL */
uint8_t YB_MAGIC[2] = {'Y', 'B'};

/*
 * Returns NULL in case of a problem
 */
YBFile *
YBFile_Open(const char *path, YBStatus *status)
{
	struct stat sb;
	if (stat(path, &sb) < 0) {
		*status = YB_ENOEXIST;
		return NULL;
	}

	FILE *fp = fopen(path, "rb"); /* XXX check for error */

	/* Validate the magic */
	char this_magic[2];
	fread(this_magic, 1, sizeof(YB_MAGIC), fp);

	for (int i = 0; i < sizeof(YB_MAGIC); i++) {
		if (this_magic[i] != YB_MAGIC[i]) {
			fclose(fp);
			*status = YB_EBADMAGIC;
			return NULL;
		}
	}

	/* Looks good -- get the metadata now */
	YBFile *yb = malloc(sizeof(YBFile));
	if (!yb) {
		fclose(fp);
		*status = YB_ENOMEM;
		return NULL;
	}
	fread(&(yb->width), sizeof(yb->width), 1, fp);
	fread(&(yb->height), sizeof(yb->height), 1, fp);

	/* Allocate memory for the raw image data */ 
	uint32_t data_start = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	uint32_t data_end = ftell(fp);
	yb->data = malloc(data_end - data_start);
	fseek(fp, data_start, SEEK_SET);
	fread(yb->data, data_end-data_start, 1, fp);
	fclose(fp);

	*status = YB_OK;
	return yb;
}


void
YBFile_Close(YBFile *yb)
{
	free(yb->data);
	free(yb);
}
