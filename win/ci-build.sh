#!/bin/bash
#
# References:
#
#	* https://www.msys2.org/docs/ci/
#
#

LOGFILE=build.log
rm -f ${LOGFILE}

die ( ) {
	[ -s $LOGFILE ] && tail $LOGFILE
	[ "$1" ] && echo "$*"
	exit -1
}

cd $(dirname $(dirname $(readlink -f ${0})))

./autogen.sh > $LOGFILE 2>&1 || die "Autogen failure"

./configure > $LOGFILE 2>&1 || die "Configure failure"

make clean > $LOGFILE 2>&1 || die "Make clean failure"

make all  > $LOGFILE 2>&1 || die "Make failure"



echo "Build complete"

