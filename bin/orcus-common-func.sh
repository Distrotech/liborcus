
orcus_exec()
{
    EXEC="$1"
    shift
    PROGDIR=`dirname $0`
    ROOTDIR=$PROGDIR/..
    EXECPATH=$ROOTDIR/src/.libs/$EXEC
    export LD_LIBRARY_PATH=$ROOTDIR/src/liborcus/.libs
    cd $ROOTDIR

    ARG_ONE=$1
    if [ "$ARG_ONE" == "gdb" ]; then
        shift
        # execute inside gdb.
        gdb --args $EXECPATH "$@" || exit 1
        exit 0
    fi

    # normal execution
    exec $EXECPATH "$@" || exit 1
    exit 0
}
