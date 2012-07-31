#!/bin/sh

PROGDIR=`dirname $0`
VERSION=`cat $PROGDIR/../VERSION`
DIR=liborcus_$VERSION

#git clone git://gitorious.org/orcus/orcus.git $DIR || exit 1
git clone file:///home/kyoshida/Documents/Workspace/orcus $DIR || exit 1
pushd . > /dev/null
cd $DIR
rm -rf .git
rm -f .gitignore
rm -rf autom4te.cache
rm -rf slickedit

popd > /dev/null

tar jcvf $DIR.tar.bz2 $DIR

