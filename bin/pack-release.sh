#!/bin/sh

PROGDIR=`dirname $0`
VERSION=`cat $PROGDIR/../VERSION`
DIR=liborcus_$VERSION
PACKAGE=$DIR.tar.bz2

#git clone git://gitorious.org/orcus/orcus.git $DIR || exit 1
git clone file:///home/kyoshida/Documents/Workspace/orcus $DIR || exit 1
pushd . > /dev/null
cd $DIR

./autogen.sh

rm -rf m4
rm -f *.m4
rm -rf .git
rm -f .gitignore
rm -rf autom4te.cache
rm -rf slickedit
rm -rf test-manual # remove large files to keep the package size small.

popd > /dev/null

tar jcvfh $PACKAGE $DIR

if [ "$1" == "md5" ]; then
    # prefix the package name with md5 sum.
    MD5SUM=`md5sum $PACKAGE | sed -e 's/\ .*//g'`
    mv $PACKAGE $MD5SUM-$PACKAGE
fi
