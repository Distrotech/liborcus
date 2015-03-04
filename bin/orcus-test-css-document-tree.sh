#!/usr/bin/env bash

PROGDIR=`dirname $0`
source $PROGDIR/orcus-common-func.sh
liborcus_test_exec liborcus-test-css-document-tree "$PROGDIR/../src/liborcus"

