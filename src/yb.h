/*
 * YB - A tiny B&W bitmap image codec thingy
 * Charlotte Koch <dressupgeekout@gmail.com>
 *
 * Fun fact: 'YB' is named after Yerin Baek for no other reason than the
 * fact I saw her in concert last night
 */

#ifndef YB_H
#define YB_H

#include <stdint.h>
#include <stdio.h>

extern uint8_t YB_MAGIC[2];

enum YBStatus {
	YB_OK,
	YB_EBADMAGIC,
	YB_ENOEXIST,
	YB_ENOMEM,
	YB_EUNKNOWN,
};
typedef enum YBStatus YBStatus;

struct YBFile {
	uint16_t width;
	uint16_t height;
	uint16_t *data;
};
typedef struct YBFile YBFile;

YBFile *YBFile_Open(const char *path, YBStatus *status);
void YBFile_Close(YBFile *yb);

#endif /* YB_H */
