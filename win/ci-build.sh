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
# Build LIBWMI++
#
echo "Building LIBWMI++"
rm -fr ./wmi
git clone https://github.com/Thomas-Sparber/wmi.git ./wmi > $LOGFILE 2>&1 || die "clone wmi++ failure"
make -C wmi all  > $LOGFILE 2>&1 || die "Make failure"
export WMI_CFLAGS="-I./wmi/include"
export WMI_LIBS="-Lwmi -lwmi -ldiaa_sami_comsupp -lwbemuuid -loleaut32 -lole32"

#
# Build VMDetect
#
echo "Building VMDETECT"
./autogen.sh > $LOGFILE 2>&1 || die "Autogen failure"
./configure > $LOGFILE 2>&1 || die "Configure failure"
make clean > $LOGFILE 2>&1 || die "Make clean failure"
make all  > $LOGFILE 2>&1 || die "Make failure"

echo "Build complete"

make DESTDIR=.bin/package install
tar --create --xz --file=${MINGW_PACKAGE_PREFIX}-vmdetect.tar.xz --directory=.bin/package --verbose .

