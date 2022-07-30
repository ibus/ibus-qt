#!/bin/bash

#if [ $# -gt 1 ] ; then
#    SRCDIR=$1
#else
#    SRCDIR="."
#fi
#if [ $# -gt 2 ] ; then
#    DSTDIR=$1
#else
#    DSTDIR=$SRCDIR
#fi
test $# -gt 1 && SRCDIR=$1 || SRCDIR="."
test $# -gt 2 && DSTDIR=$1 || DSTDIR="$SRCDIR"

VERSION_MAJOR=$(grep VERSION_MAJOR $SRCDIR/CMakeLists.txt | head -1 | awk -F\" '{print $2}')
VERSION_MINOR=$(grep VERSION_MINOR $SRCDIR/CMakeLists.txt | head -1 | awk -F\" '{print $2}')
VERSION_PATCH=$(grep VERSION_PATCH $SRCDIR/CMakeLists.txt | head -1 | awk -F\" '{print $2}')
VERSION="$VERSION_MAJOR.$VERSION_MINOR.$VERSION_PATCH"
PREV_VERSION_PATCH="$(expr $VERSION_PATCH - 1)"
PREV_VERSION="$VERSION_MAJOR.$VERSION_MINOR.$PREV_VERSION_PATCH"
echo "Compare versions $PREV_VERSION $VERSION"
zcat $DSTDIR/ibus-qt-${VERSION}-Source.tar.gz | tar xf -
wget https://github.com/ibus/ibus-qt/releases/download/$PREV_VERSION/ibus-qt-${PREV_VERSION}-Source.tar.gz
zcat ibus-qt-${PREV_VERSION}-Source.tar.gz | tar xf -
diff -r ibus-qt-${PREV_VERSION}-Source ibus-qt-${VERSION}-Source || :

