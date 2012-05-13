#!/bin/sh

EXEC=orcus-csv
PROGDIR=`dirname $0`
EXECPATH=$PROGDIR/../src/.libs/$EXEC
export LD_LIBRARY_PATH=$PROGDIR/../src/liborcus/.libs
$EXECPATH "$@"
