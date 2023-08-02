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
#echo "Building LIBWMI++"
#rm -fr ./wmi
#git clone https://github.com/Thomas-Sparber/wmi.git ./wmi > $LOGFILE 2>&1 || die "clone wmi++ failure"
#make -C wmi all  > $LOGFILE 2>&1 || die "Make failure"
#export WMI_CFLAGS="-I./wmi/include"

#
# Install pre-reqs
#
echo "Installing pre-reqs..."
pacman -U --noconfirm *.pkg.tar.zst || die "pre-reqs failure"

#
# Build VMDetect
#
echo "Building VMDETECT"

dos2unix PKGBUILD.mingw
makepkg BUILDDIR=/tmp/pkg -p PKGBUILD.mingw > $LOGFILE 2>&1 || die "makepkg failure"

echo "Build complete"


