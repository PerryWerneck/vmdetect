
dnl Process this file with autoconf to produce a configure script.

dnl The minimum version of autoconf required.
AC_PREREQ(2.59)

dnl Initialise automake with the package name, version and
dnl bug-reporting address.
AC_INIT([vmdetect], [1.3], [perry.werneck@gmail.com], [vmdetect], [https://github.com/PerryWerneck/vmdetect.git])

dnl Place auxilliary scripts here.
AC_CONFIG_AUX_DIR([scripts])

dnl Compute the canonical host-system type
AC_CANONICAL_HOST

AC_CONFIG_MACRO_DIRS([m4])

dnl Initialize libtool.
LT_INIT

dnl Put macro definitions here (though they aren't used).
AC_CONFIG_HEADER([src/include/config.h])

dnl Initialise automake stuff.
AM_INIT_AUTOMAKE

dnl Check for iconv
AM_ICONV

dnl Set gettext version
AM_GNU_GETTEXT_VERSION([0.14])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_SED
AC_PROG_LN_S

AC_LANG([C++])

dnl ---------------------------------------------------------------------------
dnl Version info
dnl ---------------------------------------------------------------------------

app_vrs_major=$(echo $VERSION | cut -d. -f1)
app_vrs_minor=$(echo $VERSION | cut -d. -f2)

AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_VERSION, $app_vrs_major, [The module major version])
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_VERSION, $app_vrs_minor, [The module minor version])

AC_SUBST(PACKAGE_MAJOR_VERSION,$app_vrs_major)
AC_SUBST(PACKAGE_MINOR_VERSION,$app_vrs_minor)

AC_ARG_WITH([release], [AS_HELP_STRING([--with-release], [Set release])], [ app_cv_release="$withval" ],[ app_cv_release="`date +%-y`.`date +%-m`.`date +%-d`" ])

app_rls_major=$(echo $app_cv_release.0.0 | cut -d. -f1)
app_rls_minor=$(echo $app_cv_release.0.0 | cut -d. -f2)

AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_RELEASE, $app_rls_major, [The library major version])
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_RELEASE, $app_rls_minor, [The library minor version])
AC_DEFINE_UNQUOTED(PACKAGE_RELEASE, "$app_cv_release", [The library release])

AC_SUBST(PACKAGE_MAJOR_RELEASE,$app_rls_major)
AC_SUBST(PACKAGE_MINOR_RELEASE,$app_rls_minor)
AC_SUBST(PACKAGE_RELEASE,$app_cv_release)

dnl ---------------------------------------------------------------------------
dnl Check for OS specifics
dnl ---------------------------------------------------------------------------

CFLAGS="$CFLAGS -I$ac_pwd/src/libvmdetect/include"
DLL_LDFLAGS="-shared -Wl,-soname,\$(@F)"

case "$host" in
	*-mingw32|*-pc-msys)
		CFLAGS="$CFLAGS -pthread "
		LDFLAGS="$LDFLAGS -pthread"
		DLL_LDFLAGS="$DLL_LDFLAGS -Wl,--output-def,\$(@D)/vmdetect.def"
		AC_SUBST(LIBEXT,.dll)
		AC_SUBST(EXEEXT,.exe)
		AC_SUBST(SONAME,libvmdetect.dll)
		AC_SUBST(OSNAME,windows)
		AC_CONFIG_FILES(sdk/windows/static.pc)
		AC_CONFIG_FILES(sdk/windows/dynamic.pc)		
		AC_PATH_TOOL([WINDRES], [windres], [no])
		AC_CONFIG_FILES(src/libvmdetect/os/windows/resources.rc)

		;;

 	*)
		CFLAGS="$CFLAGS -pthread"
		LDFLAGS="$LDFLAGS -pthread"
		AC_SUBST(LIBEXT,.so)
		AC_SUBST(EXEEXT)
		AC_SUBST(SONAME,libvmdetect.so.$app_vrs_major.$app_vrs_minor)
		AC_SUBST(OSNAME,linux)
		AC_CONFIG_FILES(sdk/linux/static.pc)
		AC_CONFIG_FILES(sdk/linux/dynamic.pc)

esac

AC_SUBST(DLL_LDFLAGS)

dnl ---------------------------------------------------------------------------
dnl Check for tools
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([VALGRIND], [valgrind], [no])
AC_PATH_TOOL([AR], [ar], [ar])
AC_PATH_TOOL([DLLTOOL], [dlltool], [dlltool])
AC_PATH_TOOL([STRIP], [strip], [true])

dnl ---------------------------------------------------------------------------
dnl Libraries
dnl ---------------------------------------------------------------------------
PKG_CHECK_EXISTS

dnl ---------------------------------------------------------------------------
dnl Check for systemd
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [SYSTEMD], [libsystemd], AC_DEFINE(HAVE_SYSTEMD,[],[Do we have systemd?]), AC_MSG_NOTICE([libsystemd not present.]) )
AC_SUBST(SYSTEMD_CFLAGS)
AC_SUBST(SYSTEMD_LIBS)

dnl ---------------------------------------------------------------------------
dnl Check for libdbus
dnl ---------------------------------------------------------------------------

dnl PKG_CHECK_MODULES( [DBUS], [dbus-1], AC_DEFINE(HAVE_DBUS,[],[Do we have d-bus?]), AC_MSG_NOTICE([ No DBUS support.]) )
AC_SUBST(DBUS_LIBS)
AC_SUBST(DBUS_CFLAGS)

dnl ---------------------------------------------------------------------------
dnl Check for wmi++
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [WMI], [wmi++], app_cv_wmi="yes", app_cv_wmi="no" )

if test "$app_cv_wmi" == "yes"; then

	AC_DEFINE(HAVE_WMI, 1, [libwmi++ is available from pkg-config])

else

	# /usr/x86_64-w64-mingw32/sys-root/mingw/include/wmi/wmi.hpp
	if test -e "$prefix/include/wmi/wmi.hpp" ; then
		AC_DEFINE(HAVE_WMI, 1, [libwmi++ is available])
		WMI_LIBS="-l:wmi.static.a -lwbemuuid -loleaut32 -lole32"
		WMI_CFLAGS="-I$prefix/include/wmi"
		AC_MSG_NOTICE([libwmi++ is available from system path.])
	fi	

fi

AC_SUBST(WMI_CFLAGS)
AC_SUBST(WMI_LIBS)

dnl ---------------------------------------------------------------------------
dnl Check for doxygen
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([DOXYGEN], [doxygen])

dnl Help line for graphviz (optionally used by doxygen)
AC_ARG_ENABLE(graphviz,
	AS_HELP_STRING([--disable-graphviz], [prevent doxygen from using dot]),
	[wantgraphviz=$enableval], [wantgraphviz=yes])

AC_CHECK_PROGS([GRAPHVIZDOT], [dot])
AC_MSG_CHECKING([whether doxygen should use graphviz])
if test "x$wantgraphviz" = "xyes" ; then
	if test $GRAPHVIZDOT ; then
		AC_MSG_RESULT([yes])
		usegraphviz=yes
	else
		AC_MSG_RESULT([no, graphviz missing])
		usegraphviz=no
	fi
else
	AC_MSG_RESULT([no, disabled by configure])
	usegraphviz=no
fi

AC_SUBST(HAVE_DOT,`echo $usegraphviz | tr yesno YESNO`)

dnl ---------------------------------------------------------------------------
dnl Check for libintl
dnl ---------------------------------------------------------------------------

AC_PATH_TOOL([XGETTEXT], [xgettext], [true])
AC_PATH_TOOL([MSGCAT], [msgcat], [true])
AC_PATH_TOOL([MSGINIT], [msginit], [true])
AC_PATH_TOOL([MSGMERGE], [msgmerge], [true])
AC_PATH_TOOL([MSGFMT], [msgfmt], [true])

dnl ---------------------------------------------------------------------------
dnl Check for C++ 2017 support
dnl ---------------------------------------------------------------------------
AC_DEFUN([AX_CHECK_COMPILE_FLAG],
[AC_PREREQ(2.59)dnl for _AC_LANG_PREFIX
AS_VAR_PUSHDEF([CACHEVAR],[ax_cv_check_[]_AC_LANG_ABBREV[]flags_$4_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler accepts $1], CACHEVAR, [
  ax_check_save_flags=$[]_AC_LANG_PREFIX[]FLAGS
  _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $4 $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
    [AS_VAR_SET(CACHEVAR,[yes])],
    [AS_VAR_SET(CACHEVAR,[no])])
  _AC_LANG_PREFIX[]FLAGS=$ax_check_save_flags])
AS_IF([test x"AS_VAR_GET(CACHEVAR)" = xyes],
  [m4_default([$2], :)],
  [m4_default([$3], :)])
AS_VAR_POPDEF([CACHEVAR])dnl
])dnl AX_CHECK_COMPILE_FLAGS

AX_CHECK_COMPILE_FLAG([-std=c++17], [CPPFLAGS="$CPPFLAGS -std=c++17"], [AC_MSG_ERROR(["C++ 17 is required"])])

dnl ---------------------------------------------------------------------------
dnl Set the ELF symbol visibility support
dnl ---------------------------------------------------------------------------
AC_DEFINE(HAVE_GNUC_VISIBILITY, 1, [supports GCC visibility attributes])

dnl ---------------------------------------------------------------------------
dnl Check for compiler visibility flag
dnl ---------------------------------------------------------------------------
AC_MSG_CHECKING(whether $CC accepts -fvisibility=hidden)
AC_CACHE_VAL(app_cv_fvisibility_ok,
[ac_save_cc="$CC"
CC="$CC -fvisibility=hidden"
AC_TRY_RUN([int main() { return 0; }],
  app_cv_fvisibility_ok=yes,
  app_cv_fvisibility_ok=no,
  app_cv_fvisibility_ok=no)
CC="$ac_save_cc"])
AC_MSG_RESULT($app_cv_fvisibility_ok)
if test $app_cv_fvisibility_ok = yes; then
  CFLAGS="$CFLAGS -fvisibility=hidden"
fi

dnl ---------------------------------------------------------------------------
dnl Check for pic
dnl ---------------------------------------------------------------------------
AC_ARG_ENABLE([pic],
	[AS_HELP_STRING([--disable-pic], [disable support for PIC libraries (required to compile on 64bits)])],
[
	app_cv_pic="$enableval"
],[

	case "$host" in
		*-mingw32)
			app_cv_pic="no"
		;;

 	s390x-*)
			app_cv_pic="yes"
		;;

	*)
		app_cv_pic="yes"
	esac
])

if test "$app_cv_pic" == "yes"; then
	CFLAGS="$CFLAGS -fPIC"
	CXXFLAGS="$CXXFLAGS -fPIC"
fi

dnl ---------------------------------------------------------------------------
dnl Check for Python
dnl ---------------------------------------------------------------------------

PKG_CHECK_MODULES( [PYTHON], [python3], app_cv_python="yes", AC_MSG_NOTICE([Python devel is unavailable]))

if test "$app_cv_python" == "yes"; then

	case "$host" in
		*-mingw32)
			BUILD_TARGETS="${BUILD_TARGETS} virtualmachine.dll"
			;;

	 	*)
			BUILD_TARGETS="${BUILD_TARGETS} virtualmachine.so"

	esac

	AC_DEFINE(HAVE_PYTHON3,1,[ Do we have python3? ])

	AC_SUBST(PYTHON_LIBS)
	AC_SUBST(PYTHON_CFLAGS)

	AC_ARG_WITH([python-sitelib], [AS_HELP_STRING([--with-python-sitelib], [Set python-sitelib])], [ app_cv_python_sitelib="$withval" ],[ app_cv_python_sitelib=`python3 -c 'import site; print(site.getsitepackages().pop(0))'` ])
	AC_DEFINE_UNQUOTED(PYTHON_SITELIB, $app_cv_python_sitelib,[ The python sitelib ])
	AC_SUBST(PYTHON_SITELIB,$app_cv_python_sitelib)

	AC_ARG_WITH([python-version], [AS_HELP_STRING([--with-python-version], [Set Python Version])], [ app_cv_python_version="$withval" ],[ app_cv_python_version=`$PKG_CONFIG --modversion python3` ])
	AC_DEFINE_UNQUOTED(PYTHON_VERSION, $app_cv_python_version,[ The python version])
	AC_SUBST(PYTHON_VERSION,$app_cv_python_version)

fi

dnl ---------------------------------------------------------------------------
dnl Configure which files to generate.
dnl ---------------------------------------------------------------------------

dnl AC_CONFIG_FILES(doxygen/doxyfile)
AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(vmdetect.pc)

dnl ---------------------------------------------------------------------------
dnl Output the generated config.status script.
dnl ---------------------------------------------------------------------------

AC_OUTPUT

