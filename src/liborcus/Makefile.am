
INCLUDES = -I$(top_srcdir)/include

EXTRA_PROGRAMS = \
	liborcus-test-string-pool \
	liborcus-test-xml-map-tree \
	liborcus-test-xml-namespace \
	liborcus-test-xml-structure-tree

if HAVE_STATIC_LIB
AM_CPPFLAGS = -D__ORCUS_STATIC_LIB=1
else
AM_CPPFLAGS = -D__ORCUS_BUILDING_DLL=1
endif

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
	string_pool.hpp \
	tokens.cpp \
	xml_context.cpp \
	xml_handler.cpp \
	xml_map_tree.cpp \
	xml_map_tree.hpp \
	xml_namespace.cpp \
	xml_parser.cpp \
	xml_simple_handler.cpp \
	xml_structure_tree.cpp

if WITH_LIBZIP

if WITH_XLSX_FILTER

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
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
	orcus_xlsx.cpp

endif # WITH_XLSX_FILTER

if WITH_ODS_FILTER

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	odf_ods_context.cpp \
	odf_ods_handler.cpp \
	odf_para_context.cpp \
	odf_tokens.cpp \
	orcus_ods.cpp

endif # WITH_ODS_FILTER

endif # WITH_LIBZIP

if WITH_GNUMERIC_FILTER

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
    	gnumeric_cell_context.cpp \
	gnumeric_context.cpp \
	gnumeric_handler.cpp \
	gnumeric_helper.cpp \
	gnumeric_sheet_context.cpp \
	gnumeric_tokens.cpp \
	orcus_gnumeric.cpp

endif # WITH_GNUMERIC_FILTER

liborcus_@ORCUS_API_VERSION@_la_CXXFLAGS = \
	$(BOOST_SYSTEM_CFLAGS) $(LIBZIP_CFLAGS) $(ZLIB_CFLAGS)
liborcus_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_@ORCUS_API_VERSION@_la_LIBADD = \
	 $(BOOST_SYSTEM_LIBS) $(BOOST_SYSTEM_LIB) $(LIBZIP_LIBS) $(ZLIB_LIBS)

# liborcus-test-string-pool

liborcus_test_string_pool_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	string_pool_test.cpp

liborcus_test_string_pool_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-map-tree

liborcus_test_xml_map_tree_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	xml_map_tree.cpp \
	xml_map_tree.hpp \
	xml_map_tree_test.cpp

liborcus_test_xml_map_tree_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-namespace

liborcus_test_xml_namespace_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	xml_namespace.cpp \
	xml_namespace_test.cpp

liborcus_test_xml_namespace_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-structure-tree

liborcus_test_xml_structure_tree_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	xml_structure_tree.cpp \
	xml_namespace.cpp \
	xml_structure_tree_test.cpp

liborcus_test_xml_structure_tree_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

TESTS = \
	liborcus-test-string-pool \
	liborcus-test-xml-map-tree \
	liborcus-test-xml-namespace \
	liborcus-test-xml-structure-tree
