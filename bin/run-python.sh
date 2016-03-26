#!/usr/bin/env bash

if [ "$1" == "" ]; then
    echo "no input file"
    exit 1
fi

RUNMODE=

if [ "$1" == "gdb" ]; then
    RUNMODE=gdb
    shift
fi

if [ ! -e "$1" ]; then
    echo "file '$1' does not exist"
    exit 1
fi

PYTHON=$(which python3)
PROGDIR=`dirname $0`
_PYTHONPATH="$PROGDIR/../src/python/.libs:$PROGDIR/../src/python"

export PYTHONPATH=$_PYTHONPATH:$PYTHONPATH
export LD_LIBRARY_PATH="$PROGDIR/../src/liborcus/.libs:$PROGDIR/../src/parser/.libs"
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH

case $RUNMODE in
    gdb)
    gdb --args $PYTHON $PWD/"$1"
        ;;
    *)
        exec $PWD/"$1"
        ;;
esac


