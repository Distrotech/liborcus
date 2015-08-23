#!/usr/bin/env bash

PROGDIR=`dirname $0`

for PYSCRIPT in $(ls $PROGDIR/*.py); do
    echo $PYSCRIPT
    exec $PROGDIR/../../bin/run-python.sh $PYSCRIPT
done

