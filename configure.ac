AC_PREREQ([2.69])
AC_INIT([dag_scheduler], [1.0], [mehoggan@gmail.com])
AM_INIT_AUTOMAKE([-Wall foreign subdir-objects])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

PKG_PROG_PKG_CONFIG
if test -z "$PKG_CONFIG"; then
  AC_MSG_WARN([Cannot find pkg-config, make sure it is installed in your PATH])
fi
# PKG_CHECK_MODULES([googletest], [googletest = 1.16.0])
PKG_CHECK_MODULES([rapidjson], [rapidjson >= 1.1])
PKG_CHECK_MODULES([YamlCPP], [yaml-cpp >= 0.8])
# PKG_CHECK_MODULES([osspUUID], [ossp-uuid >= 1.6])

# Conditionals
AC_CANONICAL_HOST()

# Detect the target system
build_linux=no
build_windows=no
build_mac=no
case "${host_os}" in
	linux*)
		build_linux=yes
		AC_CHECK_HEADERS([limits.h])
		AC_CHECK_LIB(m, sincos,
			[LIBS="-lm $LIBS" AC_DEFINE(HAVE_LIBM, 1, [Define if you have m.])],
			[AC_MSG_ERROR(
        [lib m is required - see https://www.gnu.org/software/libc/])])
    AC_PROG_CXX([g++])
    AC_MSG_RESULT(["Going to use $CXX as compiler in Linux environment."])
		;;
	darwin*)
		build_mac=yes
    AC_PROG_CXX([clang++])
    AC_MSG_RESULT(["Going to use $CXX as compiler in OSx environtment."])
    HOMEBREW_PREFIX=$(echo $HOMEBREW_PREFIX)
    AC_MSG_RESULT(["Going to use $HOMEBREW_PREFIX to find system headers"])
    AC_SUBST([HOMEBREW_PREFIX])
		;;
	*)
		AC_MSG_ERROR(["OS $host_os is not supported"])
		;;
esac
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])
LT_INIT()

AX_CXX_COMPILE_STDCXX([17], [noext], [mandatory])
AC_CONFIG_SRCDIR([lib/dag.cxx])

AC_CONFIG_FILES([
  Makefile
  lib/Makefile
  stages_lib/Makefile
  stages_lib/lib/Makefile
  test/Makefile
  bin/Makefile
])

AC_OUTPUT
