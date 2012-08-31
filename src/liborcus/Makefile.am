SUBDIRS = pre .

INCDIR = $(top_srcdir)/include
SRCDIR = $(top_srcdir)/src/liborcus
BINDIR = $(top_srcdir)/bin

INCLUDES = -I$(top_srcdir)/include

EXTRA_PROGRAMS = liborcus-test-xml-map-tree

AM_CXXFLAGS = -D__ORCUS_BUILDING_DLL=1 -fPIC

lib_LTLIBRARIES = liborcus-@ORCUS_API_VERSION@.la
liborcus_@ORCUS_API_VERSION@_la_SOURCES = \
	dom_tree.cpp \
	exception.cpp \
	global.cpp \
	spreadsheet_interface.cpp \
	orcus_css.cpp \
	orcus_csv.cpp \
	orcus_xml.cpp \
	pstring.cpp \
	string_pool.cpp \
	tokens.cpp \
	xml_context.cpp \
	xml_handler.cpp \
	xml_map_tree.cpp \
	xml_parser.cpp \
	xml_simple_handler.cpp

if ENABLE_LIBZIP

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	odf_ods_context.cpp \
	odf_ods_handler.cpp \
	odf_para_context.cpp \
	odf_tokens.cpp \
	ooxml_content_types.cpp \
	ooxml_global.cpp \
	ooxml_opc_context.cpp \
	ooxml_schemas.cpp \
	ooxml_tokens.cpp \
	ooxml_xlsx_context.cpp \
	ooxml_xlsx_handler.cpp \
	ooxml_xlsx_sheet_context.cpp \
	ooxml_xlsx_workbook_context.cpp \
	opc_reader.cpp \
	orcus_ods.cpp \
	orcus_xlsx.cpp

endif # ENABLE_LIBZIP

liborcus_test_xml_map_tree_SOURCES = \
	string_pool.cpp \
	xml_map_tree.cpp \
	xml_map_tree_test.cpp

liborcus_test_xml_map_tree_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

liborcus_@ORCUS_API_VERSION@_la_CPPFLAGS = `@PKG_CONFIG_CPPFLAGS@`
liborcus_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined `@PKG_CONFIG_LDFLAGS@`

liborcus_@ORCUS_API_VERSION@_la_LIBADD =

TESTS = liborcus-test-xml-map-tree
