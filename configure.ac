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
AM_INIT_AUTOMAKE

AC_ARG_WITH(mdds-include-path,
    AS_HELP_STRING([--with-mdds-include-path], [specify include path for mdds headers.])
,)

AC_ARG_WITH(ixion-path,
    AS_HELP_STRING([--with-ixion-path], [specify path to ixion library.])
,)

AC_ARG_ENABLE(model,
    AS_HELP_STRING([--disable-model],
        [disable the model implementation part of orcus.  Note that the command line utilities will not be built when this is disabled.])
)

ORCUS_API_VERSION=orcus_api_version
AC_SUBST(ORCUS_API_VERSION)

AC_CONFIG_MACRO_DIR([m4])

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
    test -f $top_builddir/configure && break
done

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

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
PKG_CONFIG_PATH=
LD_RUN_PATH=

AC_MSG_CHECKING([mdds include path])
if test "z$with_mdds_include_path" = "z"; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([mdds header include path not specified.])
else
    AC_MSG_RESULT([$with_mdds_include_path])
    DEFINES="$DEFINES -DMDDS_HASH_CONTAINER_BOOST"
    CPPFLAGS="-I$with_mdds_include_path"
    AC_MSG_CHECKING([usability of mdds headers])
    AC_LANG_PUSH([C++])
    AC_TRY_RUN([

#include <mdds/flat_segment_tree.hpp>

int main()
{
    return 0;
}

    ], [AC_MSG_RESULT(OK)], [AC_MSG_ERROR([mdds headers not usable.  Check your mdds path to make sure it's correct.])])
    AC_LANG_POP([C++])
fi

if test "z$enable_model" != "zno"; then

    AC_MSG_CHECKING([ixion library path])
    if test "z$with_ixion_path" = "z"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([ixion library path not specified.])
    else
        AC_MSG_RESULT([$with_ixion_path])
        PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$with_ixion_path/lib/pkgconfig:$with_ixion_path/lib64/pkgconfig
        LD_RUN_PATH=$LD_RUN_PATH:$with_ixion_path/lib:$with_ixion_path/lib64
        CXXFLAGS=`PKG_CONFIG_PATH=$PKG_CONFIG_PATH pkg-config --cflags libixion-0.4`
        AC_MSG_CHECKING([usability of ixion headers])
        AC_LANG_PUSH([C++])
        AC_TRY_RUN([

    #include <ixion/formula.hpp>

    int main()
    {
        return 0;
    }

        ], [AC_MSG_RESULT(OK)], [AC_MSG_ERROR([ixion headers not usable.  Check your ixion path to make sure it's correct.])])
        AC_LANG_POP([C++])
    fi
fi

PKG_CONFIG_CPPFLAGS="pkg-config --cflags libzip"
PKG_CONFIG_LDFLAGS="pkg-config --libs libzip"
PKG_CONFIG_CPPFLAGS_MODEL="pkg-config --cflags libixion-0.4"
PKG_CONFIG_LDFLAGS_MODEL="pkg-config --libs libixion-0.4"
CPPFLAGS="$DEFINES -Os -I./include -I$with_mdds_include_path -g -Wall"

AC_SUBST(PKG_CONFIG_PATH)
AC_SUBST(LD_RUN_PATH)
AC_SUBST(CPPFLAGS)
AC_SUBST(PKG_CONFIG_CPPFLAGS)
AC_SUBST(PKG_CONFIG_LDFLAGS)
AC_SUBST(PKG_CONFIG_CPPFLAGS_MODEL)
AC_SUBST(PKG_CONFIG_LDFLAGS_MODEL)

AM_CONDITIONAL(BUILD_MODEL, test "z$enable_model" != "zno")

AC_CONFIG_FILES([Makefile
                 liborcus.pc
                 setenv
                 include/Makefile
                 include/orcus/Makefile
                 include/orcus/model/Makefile
                 include/orcus/odf/Makefile
                 include/orcus/ooxml/Makefile
                 src/Makefile
                 src/liborcus/Makefile
                 src/model/Makefile
])

AC_OUTPUT

AC_MSG_NOTICE([completed successfully.

Before running make, run

  source ./setenv

to set up your build environment.
])
