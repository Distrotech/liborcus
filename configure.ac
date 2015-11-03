#		             -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])

# ===================
# Version information
# ===================
m4_define([orcus_major_version], [0])
m4_define([orcus_minor_version], [11])
m4_define([orcus_micro_version], [0])
m4_define([orcus_version], [orcus_major_version.orcus_minor_version.orcus_micro_version])

# ===============
# API information
# ===============
m4_define([orcus_major_api_version], [0])
m4_define([orcus_minor_api_version], [11])
m4_define([orcus_api_version], [orcus_major_version.orcus_minor_api_version])

# =============
# Automake init
# =============
AC_INIT([liborcus],[orcus_version])
AC_CONFIG_MACRO_DIR([m4])
m4_pattern_allow([^BOOST_])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11 foreign dist-bzip2 dist-xz])
AM_SILENT_RULES([yes])
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11
CXXFLAGS="$CXXFLAGS -fvisibility=hidden -Wall"

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

# ======================
# Set required ixion api
# ======================
IXION_REQUIRED_API_VERSION=0.11
AC_SUBST(IXION_REQUIRED_API_VERSION)

# =============
# Set orcus api
# =============
ORCUS_API_VERSION=orcus_api_version
ORCUS_MAJOR_VERSION=orcus_major_version
ORCUS_MINOR_VERSION=orcus_minor_version
ORCUS_MICRO_VERSION=orcus_micro_version
AC_SUBST(ORCUS_API_VERSION)
AC_SUBST(ORCUS_MAJOR_VERSION)
AC_SUBST(ORCUS_MINOR_VERSION)
AC_SUBST(ORCUS_MICRO_VERSION)

BOOST_REQUIRE([1.36])
BOOST_SYSTEM

# ==============
# Werror support
# ==============
AC_ARG_ENABLE([werror],
	[AS_HELP_STRING([--enable-werror], [Treat all warnings as errors, useful for development])],
	[enable_werror="$enableval"],
	[enable_werror=no]
)
AS_IF([test x"$enable_werror" == "xyes"], [
	CXXFLAGS="$CXXFLAGS -Werror"
])

# =========================================================
# Determine if we are going to build static lib (for MinGW)
# =========================================================
AM_CONDITIONAL([HAVE_STATIC_LIB],
               [test "$enable_shared" = no -a "$enable_static" = yes])

# =====
# Debug
# =====
AC_ARG_ENABLE(debug,
	AS_HELP_STRING([--enable-debug], [Build with debug features in mind.])],
	[enable_debug="$enableval"],
	[enable_debug=no]
)
AS_IF([test "x$enable_debug" != "xno"], [
	CXXFLAGS="$CXXFLAGS -g -O0"
], [
	CXXFLAGS="$CXXFLAGS -O2"
	AC_DEFINE([NDEBUG], [], [Disable debugging information])
])

# zlib is a hard requirement in liborcus-parser.
PKG_CHECK_MODULES([ZLIB], [zlib])

# ==============
# tools (binary)
# ==============
AC_ARG_WITH(tools,
            AS_HELP_STRING([--without-tools],
                           [Disables building of binary executable tools.]),
	[with_tools="$withval"],
	[with_tools=yes]
)

# ==================
# ods filter support
# ==================
AC_ARG_WITH(ods-filter,
            AS_HELP_STRING([--without-ods-filter],
                           [Disables the OpenDocument Format spreadsheet import filter.]),
	[with_ods_filter="$withval"],
	[with_ods_filter=yes]
)

# ===================
# xlsx filter support
# ===================
AC_ARG_WITH(xlsx-filter,
            AS_HELP_STRING([--without-xlsx-filter],
                           [Disables the Microsoft Excel OOXML import filter.]),
	[with_xlsx_filter="$withval"],
	[with_xlsx_filter=yes]
)

# ===================
# xls xml filter support
# ===================
AC_ARG_WITH(xls-xml-filter,
            AS_HELP_STRING([--without-xls-xml-filter],
                           [Disables the Microsoft Excel XML import filter.]),
	[with_xls_xml_filter="$withval"],
	[with_xls_xml_filter=yes]
)

# =======================
# gnumeric filter support
# =======================
AC_ARG_WITH(gnumeric-filter,
            AS_HELP_STRING([--without-gnumeric-filter],
                           [Disables the gnumeric import filter. The gnumeric import filter depends on zLib.]),
	[with_gnumeric_filter="$withval"],
	[with_gnumeric_filter=yes]
)

AM_CONDITIONAL([WITH_ODS_FILTER], [test "x$with_ods_filter" != "xno"])
AM_CONDITIONAL([WITH_XLSX_FILTER], [test "x$with_xlsx_filter" != "xno"])
AM_CONDITIONAL([WITH_XLS_XML_FILTER], [test "x$with_xls_xml_filter" != "xno"])
AM_CONDITIONAL([WITH_GNUMERIC_FILTER], [test "x$with_gnumeric_filter" != "xno"])
AM_CONDITIONAL([WITH_TOOLS], [test "x$with_tools" != "xno"])

AS_IF([test "x$with_gnumeric_filter" != "xno"], [
        BOOST_IOSTREAMS
])

AS_IF([test "x$with_tools" != "xno"], [
        BOOST_PROGRAM_OPTIONS
        BOOST_FILESYSTEM
        BOOST_SYSTEM
])

# ============
# mdds support
# ============
PKG_CHECK_MODULES([MDDS],[mdds-1.0 >= 1.0.0])
CXXFLAGS="$CXXFLAGS -DMDDS_HASH_CONTAINER_BOOST $MDDS_CFLAGS"

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
	PKG_CHECK_MODULES([LIBIXION],[libixion-$IXION_REQUIRED_API_VERSION])
])
AM_CONDITIONAL([BUILD_SPREADSHEET_MODEL], [test "x$enable_spreadsheet_model" != "xno"])

# ==============
# Python support
# ==============
AC_ARG_ENABLE([python],
    [AS_HELP_STRING([--disable-python], [Disable python bindings])],
    [enable_python="$enableval"],
    [enable_python=yes]
)

# Check for python.
AS_IF([test "x$enable_python" != "xno"], [
    AM_PATH_PYTHON(3)
    PKG_CHECK_MODULES([PYTHON], [python3 >= 0.28])
])
AM_CONDITIONAL([BUILD_PYTHON], [test "x$enable_python" != "xno"])

AC_CONFIG_FILES([Makefile
	liborcus-$ORCUS_API_VERSION.pc:liborcus.pc.in
	liborcus-spreadsheet-model-$ORCUS_API_VERSION.pc:liborcus-spreadsheet-model.pc.in
	VERSION
	include/Makefile
	include/orcus/Makefile
	include/orcus/mso/Makefile
	include/orcus/spreadsheet/Makefile
	src/Makefile
	src/liborcus/Makefile
        src/liborcus/constants.inl
	src/mso/Makefile
	src/parser/Makefile
        src/python/Makefile
	src/spreadsheet/Makefile
        parser_handlers/Makefile
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
	spreadsheet-model:   $enable_spreadsheet_model
	python:              $enable_python
        gnumeric-filter:     $with_gnumeric_filter
        ods-filter:          $with_ods_filter
        xlsx-filter:         $with_xlsx_filter
        xls-xml-filter:      $with_xls_xml_filter
        tools:               $with_tools
==============================================================================
])

