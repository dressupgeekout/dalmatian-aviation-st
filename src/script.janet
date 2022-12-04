# DALMATIAN AVIATION ST
# Dialogue script compiler

(def ACCUMULATOR @[])

(def KRISSANY 1) 
(def WHITNEY 2) 
### XXX (string/from-bytes KRISSANY)

(defn krissany
  [line]
  (array/push ACCUMULATOR 
    (string/format "%s" line)))

(defn whitney
  [line]
  (array/push ACCUMULATOR
    (string/format "%s" line)))

########## ########## ##########

(defn do-the-script
  []
    (krissany `This is the first line of dialogue!`)
    (krissany `This is another line of dialogue. I am awesome.`)
    (whitney `Are you sure about that?`)
    (krissany `Sure I'm sure. I know things!`))

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

#include <stdint.h>

#include "dialogue.h"

Dialogue DIALOGUE_LINES[] = {
  ```)
  (print ```
  ```)

  (loop [line :in ACCUMULATOR]
    (printf "\t{%d, %q}," 0 line))

  (print ```
};

#endif /* DALMATIAN_SCRIPT_H */
  ```))

########## ########## ##########

(compiler-main)
