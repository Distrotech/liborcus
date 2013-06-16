#!/bin/sh

PROGDIR=`dirname $0`
source $PROGDIR/orcus-common-func.sh
orcus_exec orcus-xls-xml "$PWD" "$@"
