#!/bin/sh
set -ex
make
cp DALMATIA.PRG ~/atariroot/DALMATIA
cp BLIMP2.YB ~/atariroot/DALMATIA/BLIMP2.YB
cp GEARS2.YB ~/atariroot/DALMATIA/GEARS2.YB
cp TEST.JAN ~/atariroot/DALMATIA/TEST.JAN
cp TALK.JAN ~/atariroot/DALMATIA/TALK.JAN
cp ARTIF.JAN ~/atariroot/DALMATIA/ARTIF.JAN
