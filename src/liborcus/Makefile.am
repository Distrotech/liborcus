
INCLUDES = -I$(top_srcdir)/include -I./include

EXTRA_PROGRAMS = \
	liborcus-test-xml-map-tree \
	liborcus-test-xml-structure-tree \
	liborcus-test-common \
	liborcus-test-format-detection

TESTS =

if HAVE_STATIC_LIB
AM_CPPFLAGS = -D__ORCUS_STATIC_LIB=1
else
AM_CPPFLAGS = -D__ORCUS_BUILDING_DLL=1
endif

liborcus_@ORCUS_API_VERSION@_la_CXXFLAGS = \
	$(BOOST_SYSTEM_CFLAGS) $(ZLIB_CFLAGS)
liborcus_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_@ORCUS_API_VERSION@_la_LIBADD = \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	 $(BOOST_SYSTEM_LIBS) $(BOOST_SYSTEM_LIB) $(ZLIB_LIBS)

AM_CPPFLAGS += -DSRCDIR=\""$(top_srcdir)"\"

lib_LTLIBRARIES = liborcus-@ORCUS_API_VERSION@.la
liborcus_@ORCUS_API_VERSION@_la_SOURCES = \
	detection_result.hpp \
	detection_result.cpp \
	dom_tree.cpp \
	format_detection.cpp \
	global.cpp \
	interface.cpp \
	spreadsheet_interface.cpp \
	orcus_css.cpp \
	orcus_csv.cpp \
	orcus_xml.cpp \
	measurement.cpp \
	tokens.cpp \
	xml_context_base.hpp \
	xml_context_base.cpp \
	xml_context_global.hpp \
	xml_context_global.cpp \
	xml_map_tree.hpp \
	xml_map_tree.cpp \
	xml_stream_handler.hpp \
	xml_stream_handler.cpp \
	xml_stream_parser.hpp \
	xml_stream_parser.cpp \
	xml_simple_stream_handler.hpp \
	xml_simple_stream_handler.cpp \
	xml_structure_tree.cpp \
	ooxml_namespace_types.cpp \
	ooxml_namespace_types.hpp \
	odf_namespace_types.hpp \
	odf_namespace_types_hpp.inl \
	odf_namespace_types.cpp \
	odf_namespace_types_cpp.inl \
	gnumeric_namespace_types.hpp \
	gnumeric_namespace_types.cpp \
	xls_xml_namespace_types.hpp \
	xls_xml_namespace_types.cpp

if WITH_XLSX_FILTER

AM_CPPFLAGS += -D__ORCUS_XLSX
EXTRA_PROGRAMS += \
	orcus-xlsx-sheet-context-test

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	session_context.hpp \
	ooxml_content_types.cpp \
	ooxml_content_types.hpp \
	ooxml_global.cpp \
	ooxml_global.hpp \
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

# orcus-xlsx-sheet-context-test

orcus_xlsx_sheet_context_test_SOURCES = \
	xlsx_sheet_context_test.cpp \
	xlsx_sheet_context.cpp \
	ooxml_global.cpp \
	ooxml_namespace_types.cpp \
	ooxml_tokens.cpp \
	ooxml_schemas.cpp \
	xml_context_base.cpp \
	tokens.cpp \
	global.cpp \
	spreadsheet_interface.cpp \
	mock_spreadsheet.hpp \
	mock_spreadsheet.cpp

orcus_xlsx_sheet_context_test_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_xlsx_sheet_context_test_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS) -DNO_XML_EXPECTED

TESTS += \
	 orcus-xlsx-sheet-context-test


endif # WITH_XLSX_FILTER

if WITH_XLS_XML_FILTER

AM_CPPFLAGS += -D__ORCUS_XLS_XML

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	xls_xml_tokens.hpp \
	xls_xml_tokens.inl \
	xls_xml_tokens.cpp \
	xls_xml_token_constants.hpp \
	xls_xml_token_constants.inl \
	orcus_xls_xml.cpp \
	xls_xml_detection_handler.hpp \
	xls_xml_detection_handler.cpp \
	xls_xml_handler.hpp \
	xls_xml_handler.cpp \
	xls_xml_context.hpp \
	xls_xml_context.cpp

endif # WITH_XLS_XML_FILTER

if WITH_ODS_FILTER

AM_CPPFLAGS += -D__ORCUS_ODS

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	session_context.hpp \
	odf_para_context.hpp \
	odf_para_context.cpp \
	odf_styles.hpp \
	odf_styles.cpp \
	odf_styles_context.hpp \
	odf_styles_context.cpp \
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

if WITH_GNUMERIC_FILTER

AM_CPPFLAGS += -D__ORCUS_GNUMERIC

liborcus_@ORCUS_API_VERSION@_la_SOURCES += \
	session_context.hpp \
	gnumeric_cell_context.hpp \
	gnumeric_context.hpp \
	gnumeric_detection_handler.hpp \
	gnumeric_detection_handler.cpp \
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
	gnumeric_tokens.inl \
	orcus_gnumeric.cpp

liborcus_@ORCUS_API_VERSION@_la_LDFLAGS += \
	$(BOOST_IOSTREAMS_LDFLAGS)

liborcus_@ORCUS_API_VERSION@_la_LIBADD += \
	$(BOOST_IOSTREAMS_LIBS)

# gnumeric-cell-context-test

EXTRA_PROGRAMS += \
	gnumeric-cell-context-test \
	orcus-gnumeric-sheet-context-test

gnumeric_cell_context_test_SOURCES = \
	gnumeric_cell_context_test.cpp \
	gnumeric_cell_context.cpp \
	xml_context_base.cpp \
	gnumeric_namespace_types.cpp \
	tokens.cpp \
	gnumeric_tokens.cpp \
	odf_namespace_types.cpp \
	spreadsheet_interface.cpp \
	mock_spreadsheet.hpp \
	mock_spreadsheet.cpp

gnumeric_cell_context_test_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

orcus_gnumeric_cell_context_test_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

TESTS += gnumeric-cell-context-test

# orcus-gnumeric-sheet-context-test

orcus_gnumeric_sheet_context_test_SOURCES = \
	gnumeric_sheet_context_test.cpp \
	gnumeric_sheet_context.cpp \
	gnumeric_cell_context.cpp \
	gnumeric_helper.cpp \
	xml_context_base.cpp \
	gnumeric_namespace_types.cpp \
	tokens.cpp \
	gnumeric_tokens.cpp \
	odf_namespace_types.cpp \
	spreadsheet_interface.cpp \
	mock_spreadsheet.hpp \
	mock_spreadsheet.cpp

orcus_gnumeric_sheet_context_test_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

orcus_gnumeric_sheet_context_test_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

TESTS += orcus-gnumeric-sheet-context-test

endif # WITH_GNUMERIC_FILTER

# liborcus-test-xml-map-tree

liborcus_test_xml_map_tree_SOURCES = \
	xml_map_tree.cpp \
	xml_map_tree.hpp \
	xml_map_tree_test.cpp

liborcus_test_xml_map_tree_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-xml-structure-tree

liborcus_test_xml_structure_tree_SOURCES = \
	xml_structure_tree.cpp \
	xml_structure_tree_test.cpp

liborcus_test_xml_structure_tree_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS) -DSRCDIR=\""$(top_srcdir)"\"

liborcus_test_xml_structure_tree_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-common

liborcus_test_common_SOURCES = \
	global.cpp \
	tokens.cpp \
	common_test.cpp

liborcus_test_common_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

# liborcus-test-format-detection

liborcus_test_format_detection_SOURCES = \
	format_detection_test.cpp

liborcus_test_format_detection_LDADD = \
	liborcus-@ORCUS_API_VERSION@.la

TESTS += \
	liborcus-test-xml-map-tree \
	liborcus-test-xml-structure-tree \
	liborcus-test-common \
	liborcus-test-format-detection

distclean-local:
	rm -rf $(TESTS)

