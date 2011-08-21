#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([orcus_major_version], [0])
m4_define([orcus_minor_version], [1])
m4_define([orcus_micro_version], [0])
m4_define([orcus_version],
                    [orcus_major_version.orcus_minor_version.orcus_micro_version])
m4_define([orcus_minor_api_version], [m4_eval(orcus_minor_version + orcus_minor_version%2)])
m4_define([orcus_api_version],
                    [orcus_major_version.orcus_minor_api_version])

AC_PREREQ([2.63])
AC_INIT([orcus], [orcus_version])

AC_ARG_WITH(mdds-include-path,
    AS_HELP_STRING([--with-mdds-include-path], [specify include path for mdds headers.])
,)

ORCUS_API_VERSION=orcus_api_version
AC_SUBST(ORCUS_API_VERSION)

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday])

DEFINES="-D_REENTRANT"
AC_MSG_CHECKING([mdds include path])
if test "z$with_mdds_include_path" = "z"; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([mdds header include path not specified.])
else
    AC_MSG_RESULT([$with_mdds_include_path])
    DEFINES="$DEFINES -DMDDS_HASH_CONTAINER_BOOST"
fi

CPPFLAGS="$DEFINES -Os -I./include -I$with_mdds_include_path -g -Wall `pkg-config --cflags libzip` -std=c++0x"
LDFLAGS="`pkg-config --libs libzip`"

AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AC_OUTPUT(Makefile)
