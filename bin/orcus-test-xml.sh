#!/bin/sh

EXEC=orcus-test-xml
PROGDIR=`dirname $0`
SRCDIR=$PROGDIR/../src
EXECPATH=./.libs/$EXEC
export LD_LIBRARY_PATH=./liborcus/.libs
cd $SRCDIR

ARG_ONE=$1
if [ "$ARG_ONE" == "" ]; then
    # normal execution
    exec $EXECPATH "$@" || exit 1
    exit 0
fi

shift

if [ "$ARG_ONE" == "gdb" ]; then
    # execute inside gdb.
    gdb --args $EXECPATH "$@"
fi

