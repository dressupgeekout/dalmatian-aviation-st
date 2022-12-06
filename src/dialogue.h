/*
 * DALMATIAN AVIATION ST
 * dialogue.h
 */

#ifndef DIALOGUE_H
#define DIALOGUE_H

struct Dialogue {
  uint8_t character;
  const char *line;
	const char *instructions;
};
typedef struct Dialogue Dialogue;

/* GLOBAL VARIABLES */
extern const char *CHARACTER_MAP[];
extern Dialogue DIALOGUE_LINES[];

#endif /* DIALOGUE_H */
