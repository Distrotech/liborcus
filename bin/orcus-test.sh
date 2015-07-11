#!/usr/bin/env bash

LOCATION=
case "$1" in
    common|css-document-tree|json-document-tree|format-detection|xml-map-tree|xml-structure-tree)
        LOCATION=liborcus
        ;;
    csv|ods|xml|xml-mapped|xls-xml|xlsx)
        LOCATION=orcus
        ;;
    base64|sax-token-parser|string-pool|xml-namespace)
        LOCATION=parser
        ;;
esac

if [ "$LOCATION" = "" ]; then

    echo ""
    echo "Specify which test to run by passing one of"
    echo ""
    echo "  * csv"
    echo "  * ods"
    echo "  * xml"
    echo "  * xml-mapped"
    echo "  * xls-xml"
    echo "  * xlsx"
    echo
    echo "  * common"
    echo "  * css-document-tree"
    echo "  * json-document-tree"
    echo "  * format-detection"
    echo "  * xml-map-tree"
    echo "  * xml-structure-tree"
    echo
    echo "  * base64"
    echo "  * sax-token-parser"
    echo "  * string-pool"
    echo "  * xml-namespace"
    echo ""

    exit 1
fi

PROGDIR=`dirname $0`
source $PROGDIR/orcus-common-func.sh
TEST=$LOCATION-test-$1
echo "Running $TEST..."

case $LOCATION in
    liborcus)
        liborcus_test_exec $TEST "$PROGDIR/../src/liborcus"
        ;;
    orcus)
        orcus_exec $TEST "$PROGDIR/../src/"
        ;;
    parser)
        parser_test_exec $TEST "$PROGDIR/../src/parser"
esac
