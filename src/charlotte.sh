#!/bin/sh
set -ex
make
cp DALMATIA.PRG ~/atariroot/DALMATIA
rsync -avr ./script/ ~/atariroot/DALMATIA/SCRIPT/
