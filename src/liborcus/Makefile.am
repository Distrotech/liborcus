
INCLUDES = -I$(top_srcdir)/include -I./include

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
	string_pool.hpp \
	string_pool.cpp \
	tokens.cpp \
	xml_context_base.hpp \
	xml_context_base.cpp \
	xml_map_tree.hpp \
	xml_map_tree.cpp \
	xml_namespace.cpp \
	xml_stream_handler.hpp \
	xml_stream_handler.cpp \
	xml_stream_parser.hpp \
	xml_stream_parser.cpp \
	xml_simple_stream_handler.hpp \
	xml_simple_stream_handler.cpp \
	xml_structure_tree.cpp

if WITH_LIBZIP

if WITH_XLSX_FILTER

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	ooxml_content_types.cpp \
	ooxml_content_types.hpp \
	ooxml_global.cpp \
	ooxml_global.hpp \
	ooxml_namespace_types.cpp \
	ooxml_namespace_types.hpp \
	ooxml_schemas.cpp \
	ooxml_schemas.hpp \
	ooxml_token_constants.hpp \
	ooxml_token_constants.inl \
	ooxml_tokens.cpp \
	ooxml_tokens.hpp \
	ooxml_tokens.inl \
	ooxml_types.hpp \
	opc_context.cpp \
	opc_context.hpp \
	opc_reader.cpp \
	opc_reader.hpp \
	opc_reader.hpp \
	opc_token_constants.hpp \
	opc_token_constants.inl \
	opc_tokens.inl \
	orcus_xlsx.cpp \
	xlsx_context.cpp \
	xlsx_context.hpp \
	xlsx_handler.cpp \
	xlsx_handler.hpp \
	xlsx_sheet_context.cpp \
	xlsx_sheet_context.hpp \
	xlsx_types.hpp \
	xlsx_workbook_context.cpp \
	xlsx_workbook_context.hpp

endif # WITH_XLSX_FILTER

if WITH_ODS_FILTER

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	odf_para_context.hpp \
	odf_para_context.cpp \
	odf_token_constants.hpp \
	odf_token_constants.inl \
	odf_tokens.hpp \
	odf_tokens.inl \
	odf_tokens.cpp \
	ods_content_xml_context.hpp \
	ods_content_xml_context.cpp \
	ods_content_xml_handler.hpp \
	ods_content_xml_handler.cpp \
	orcus_ods.cpp

endif # WITH_ODS_FILTER

endif # WITH_LIBZIP

if WITH_GNUMERIC_FILTER

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	gnumeric_cell_context.hpp \
	gnumeric_context.hpp \
	gnumeric_handler.hpp \
	gnumeric_helper.hpp \
	gnumeric_sheet_context.hpp \
	gnumeric_token_constants.hpp \
	gnumeric_token_constants.inl \
	gnumeric_tokens.hpp \
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
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-map-tree

liborcus_test_xml_map_tree_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	xml_map_tree.cpp \
	xml_map_tree.hpp \
	xml_map_tree_test.cpp

liborcus_test_xml_map_tree_LDADD = \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-namespace

liborcus_test_xml_namespace_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	xml_namespace.cpp \
	xml_namespace.hpp \
	ooxml_namespace_types.hpp \
	ooxml_namespace_types.cpp \
	xml_namespace_test.cpp

liborcus_test_xml_namespace_LDADD = \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-structure-tree

liborcus_test_xml_structure_tree_SOURCES = \
	string_pool.cpp \
	string_pool.hpp \
	xml_structure_tree.cpp \
	xml_namespace.cpp \
	xml_structure_tree_test.cpp

liborcus_test_xml_structure_tree_LDADD = \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus-@ORCUS_API_VERSION@.la

TESTS = \
	liborcus-test-string-pool \
	liborcus-test-xml-map-tree \
	liborcus-test-xml-namespace \
	liborcus-test-xml-structure-tree
