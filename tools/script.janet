# DALMATIAN AVIATION ST
# Dialogue script compiler

(def __ACCUMULATOR @[]) # list of beats: [speaker, line]
(def __CHARACTERS @[]) # tuples (speaker-name, id)

(defn declare-character
  [name id]
  (array/push __CHARACTERS (tuple name id)))

########## ########## ##########

(declare-character "KRISSANY" 0)
(declare-character "WHITNEY" 1)
(declare-character "BORIS" 2)

(defn krissany
  [line]
  (array/push __ACCUMULATOR  ["KRISSANY" line]))

(defn whitney
  [line]
  (array/push __ACCUMULATOR ["WHITNEY" line]))

(defn boris
  [line]
  (array/push __ACCUMULATOR ["BORIS" line]))

########## ########## ##########

(defn do-the-script
  []
  (krissany `This is the first line of dialogue!`)
  (krissany `This is another line of dialogue. I am awesome.`)
  (whitney `Are you sure about that?`)
  (krissany `Sure I'm sure. I know things!`)
  (boris `Wot's this then?`)
  (krissany `Beats me!`))

########## ########## ##########

(defn compiler-main
  []
  (do-the-script)

  (print ```
/*
 * AUTOMATICALLY GENERATED, DO NOT EDIT.
 */

#ifndef DALMATIAN_SCRIPT_H
#define DALMATIAN_SCRIPT_H

#include "dialogue.h"

```)

  (let [characters-by-id (sorted-by (fn [entry] (get entry 1)) __CHARACTERS)]
    (loop [[name id] :in characters-by-id]
      (printf "#define CHAR_%s %d" name id))

    (print `const char *CHARACTER_MAP[] = {`)

    (loop [[name id] :in characters-by-id]
      (printf "\t%q," name)))

  (print `};`)
  (printf "#define NUM_BEATS %d" (length __ACCUMULATOR))
  (print `Dialogue DIALOGUE_LINES[] = {`)

  (loop [[speaker line] :in __ACCUMULATOR]
    (printf "\t{CHAR_%s, %q, NULL}," speaker line))

  (print ```
};

#endif /* DALMATIAN_SCRIPT_H */
  ```))

########## ########## ##########

(compiler-main)
