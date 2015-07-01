#!/usr/bin/env bash

if [ "$1" == "" ]; then
    echo "no input file"
    exit 1
fi

if [ ! -e "$1" ]; then
    echo "file '$1' does not exist"
    exit 1
fi

PROGDIR=`dirname $0`
_PYTHONPATH="$PROGDIR/../src/python/.libs"

export PYTHONPATH=$_PYTHONPATH:$PYTHONPATH
exec $PWD/"$1"


