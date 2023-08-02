#!/bin/bash
#
# References:
#
#	* https://www.msys2.org/docs/ci/
#
#
echo "Running ${0}"

LOGFILE=build.log
rm -f ${LOGFILE}

die ( ) {
	[ -s $LOGFILE ] && tail $LOGFILE
	[ "$1" ] && echo "$*"
	exit -1
}

cd $(dirname $(dirname $(readlink -f ${0})))

#
# Build VMDetect
#
echo "Building VMDETECT"

dos2unix PKGBUILD.mingw
makepkg BUILDDIR=/tmp/pkg -p PKGBUILD.mingw > $LOGFILE 2>&1 || die "makepkg failure"

echo "Build complete"


