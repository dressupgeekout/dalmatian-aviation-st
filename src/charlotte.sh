#!/bin/sh
set -ex
make
cp DALMATIA.PRG ~/atariroot/DALMATIA
cp blimp2.bw ~/atariroot/DALMATIA/BLIMP2.BW
cp gears2.bw ~/atariroot/DALMATIA/GEARS2.BW
