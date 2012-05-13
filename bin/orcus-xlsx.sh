#!/bin/sh

EXEC=orcus-xlsx
PROGDIR=`dirname $0`
EXECPATH=$PROGDIR/../src/.libs/$EXEC
export LD_LIBRARY_PATH=$PROGDIR/../src/liborcus/.libs
$EXECPATH "$@"
