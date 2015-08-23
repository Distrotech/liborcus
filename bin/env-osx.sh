
# Source this to set the debug environment for OSX.

PROGDIR=`dirname $0`
ROOTDIR="$PROGDIR/.."

PARSER_DIR="$ROOTDIR/src/parser/.libs"
LIBORCUS_DIR="$ROOTDIR/src/liborcus/.libs"
SPREADSHEET_DIR="$ROOTDIR/src/spreadsheet/.libs"

LD_PATH="$PARSER_DIR:$LIBORCUS_DIR:$SPREADSHEET_DIR"

export DYLD_LIBRARY_PATH="$LD_PATH"



