#!/usr/bin/env bash

LOCATION=
NAME=$BASH_ARGV
case $NAME in
    common|*-document-tree|format-detection|xml-map-tree|xml-structure-tree)
        LOCATION=liborcus
        ;;
    csv|ods|xml|xml-mapped|xls-xml|xlsx)
        LOCATION=orcus
        ;;
    base64|sax-token-parser|stream|string-pool|xml-namespace|zip-archive)
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
    echo "  * yaml-document-tree"
    echo "  * format-detection"
    echo "  * xml-map-tree"
    echo "  * xml-structure-tree"
    echo
    echo "  * base64"
    echo "  * sax-token-parser"
    echo "  * stream"
    echo "  * string-pool"
    echo "  * xml-namespace"
    echo "  * zip-archive"
    echo ""

    exit 1
fi

PROGDIR=`dirname $0`
source $PROGDIR/orcus-common-func.sh
TEST=$LOCATION-test-$NAME
echo "Running $TEST..."

case $LOCATION in
    liborcus)
        liborcus_test_exec $TEST "$PROGDIR/../src/liborcus" "$@"
        ;;
    orcus)
        orcus_exec $TEST "$PROGDIR/../src/" "$@"
        ;;
    parser)
        parser_test_exec $TEST "$PROGDIR/../src/parser" "$@"
esac
