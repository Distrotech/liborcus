#!/usr/bin/env bash

PROGDIR=`dirname $0`
source $PROGDIR/orcus-common-func.sh
orcus_exec orcus-xml-dump "$PWD" "$@"
