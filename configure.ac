#		             -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])

# ====================
# Version informations
# ====================
m4_define([orcus_major_version], [0])
m4_define([orcus_minor_version], [1])
m4_define([orcus_micro_version], [0])
m4_define([orcus_version], [orcus_major_version.orcus_minor_version.orcus_micro_version])

# ================
# API informations
# ================
m4_define([orcus_minor_api_version], [m4_eval(orcus_minor_version + orcus_minor_version%2)])
m4_define([orcus_api_version], [orcus_major_version.orcus_minor_api_version])

# =============
# Automake init
# =============
AC_INIT([orcus],[orcus_version])
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])
AC_LANG([C++])
CXXFLAGS="$CXXFLAGS -O2 -fvisibility=hidden -Wall"

# ===========================
# Find required base packages
# ===========================
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
AC_LIBTOOL_WIN32_DLL
PKG_PROG_PKG_CONFIG([0.20])

# =====================
# Find required headers
# =====================
AC_CHECK_HEADERS([stdlib.h sys/time.h unistd.h])

# =============================================================
# Checks for typedefs, structures, and compiler characteristics
# =============================================================
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_SIZE_T

# ============================
# Checks for library functions
# ============================
AC_FUNC_STRTOD
AC_CHECK_FUNCS([gettimeofday])
AC_DEFINE([_REENTRANT], [], [Enable reentrant.])
AC_DEFINE([BOOST_ALL_NO_LIB], [], [Disable boost's evil autolink.])

# =============
# Set orcus api
# =============
ORCUS_API_VERSION=orcus_api_version
AC_SUBST(ORCUS_API_VERSION)

# ==============
# Werror support
# ==============
AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--disable-werror], [Treat all warnings as errors, useful for development])],
	[enable_werror="$enableval"],
	[enable_werror=yes]
)
AS_IF([test x"$enable_werror" != "xno"], [
	CXXFLAGS="$CXXFLAGS -Werror"
])

# =====
# Debug
# =====
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug], [Build with debug features in mind.])],
	[enable_debug="$enableval"],
	[enable_debug=no]
)
AS_IF([test "x$enable_debug" != "xno"], [
	CXXFLAGS="$CXXFLAGS -g"
], [
	AC_DEFINE([NDEBUG], [], [Disable debugging information])
])

# ==============
# libzip support
# ==============
AC_ARG_WITH(libzip,
	AS_HELP_STRING([--without-libzip],
		[Disable libzip. Support for all zip-storage based formats will be disabled if libzip is disabled.])],
	[with_libzip="$withval"],
	[with_libzip=yes]
)
AS_IF([test "x$with_libzip" != "xno"], [
	PKG_CHECK_MODULES([LIBZIP], [libzip])
])
AM_CONDITIONAL([WITH_LIBZIP], [test "x$with_libzip" != "xno"])

# ============
# mdds support
# ============
AC_ARG_WITH(mdds-include-path,
	AS_HELP_STRING([--with-mdds-include-path], [Path for mdds, if different default [/usr/include].])],
	[with_mdds_include_path="$withval"],
	[with_mdds_include_path=default]
,)
AS_IF([test "x$with_mdds_include_path" != "xdefault"], [
	# Check with other include path in mind
	MDDS_CXXFLAGS="-I${with_mdds_include_path}"
])
# define for boost container
MDDS_CPPFLAGS="-DMDDS_HASH_CONTAINER_BOOST"

# Try to check for our desired header
#save_CPPFLAGS="$CPPFLAGS"
#save_CXXFLAGS="$CXXFLAGS"
CPPFLAGS="$CPPFLAGS $MDDS_CPPFLAGS"
CXXFLAGS="$CXXFLAGS $MDDS_CXXFLAGS"
AC_CHECK_HEADERS([mdds/flat_segment_tree.hpp])
# We do not restore here as we use mdds with this settings during the build.
# Keeping the command here so none gets fancy ideas like adding it.
#CPPFLAGS="$save_CPPFLAGS"
#CXXFLAGS="$save_CXXFLAGS"

# =================
# Spreadsheet model
# =================
AC_ARG_ENABLE(spreadsheet-model,
	AS_HELP_STRING([--disable-spreadsheet-model],
		[Disable the spreadsheet model implementation in orcus.  Note that the spreadsheet-specific command line utilities will not be built when this is disabled.])],
	[enable_spreadsheet_model="$enableval"],
	[enable_spreadsheet_model=yes]
)
AS_IF([test "x$enable_spreadsheet_model" != "xno"], [
	PKG_CHECK_MODULES([LIBIXION],[libixion-0.6])
])
AM_CONDITIONAL([BUILD_SPREADSHEET_MODEL], [test "x$enable_spreadsheet_model" != "xno"])

AC_CONFIG_FILES([Makefile
	liborcus.pc
	VERSION
	include/Makefile
	include/orcus/Makefile
	include/orcus/spreadsheet/Makefile
	include/orcus/odf/Makefile
	include/orcus/ooxml/Makefile
	src/Makefile
	src/liborcus/pre/Makefile
	src/liborcus/Makefile
	src/spreadsheet/Makefile
])
AC_OUTPUT

# ==============================================
# Display final informations about configuration
# ==============================================
AC_MSG_NOTICE([
==============================================================================
Build configuration:
	debug:               $enable_debug
	werror:              $enable_werror
	mdds-include-dir:    $with_mdds_include_path
	libzip:              $with_libzip
	spreadsheet-model:   $enable_spreadsheet_model
==============================================================================
])

