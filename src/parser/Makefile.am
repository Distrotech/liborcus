
AM_CPPFLAGS = -I$(top_srcdir)/include $(BOOST_CPPFLAGS) -D__ORCUS_PSR_BUILDING_DLL

lib_LTLIBRARIES = liborcus-parser-@ORCUS_API_VERSION@.la
liborcus_parser_@ORCUS_API_VERSION@_la_SOURCES = \
	win_stdint.h \
	base64.cpp \
	cell_buffer.cpp \
	css_parser_base.cpp \
	css_types.cpp \
	csv_parser_base.cpp \
	exception.cpp \
	json_parser_base.cpp \
	json_parser_thread.cpp \
	parser_base.cpp \
	parser_global.cpp \
	pstring.cpp \
	sax_parser_base.cpp \
	sax_token_parser.cpp \
	stream.cpp \
	string_pool.cpp \
	tokens.cpp \
	types.cpp \
	xml_namespace.cpp \
	yaml_parser_base.cpp \
	zip_archive.cpp \
	zip_archive_stream.cpp


liborcus_parser_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined $(BOOST_SYSTEM_LDFLAGS)
liborcus_parser_@ORCUS_API_VERSION@_la_LIBADD = \
	$(BOOST_SYSTEM_LIBS) \
	$(ZLIB_LIBS)

EXTRA_PROGRAMS = \
	parser-test-string-pool \
	parser-test-xml-namespace \
	parser-test-base64 \
	parser-test-sax-token-parser \
	parser-test-stream \
	parser-test-threaded-json-parser \
	parser-test-zip-archive

# parser-test-string-pool

parser_test_string_pool_SOURCES = \
	string_pool.cpp \
	string_pool_test.cpp

parser_test_string_pool_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_string_pool_CPPFLAGS = $(AM_CPPFLAGS)

# parser-test-xml-namespace

parser_test_xml_namespace_SOURCES = \
	xml_namespace.cpp \
	xml_namespace_test.cpp

parser_test_xml_namespace_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_xml_namespace_CPPFLAGS = $(AM_CPPFLAGS)

# parser-test-base64

parser_test_base64_SOURCES = \
	base64.cpp \
	base64_test.cpp

parser_test_base64_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_base64_CPPFLAGS = $(AM_CPPFLAGS)

# parser-test-sax-token-parser

parser_test_sax_token_parser_SOURCES = \
	sax_token_parser_test.cpp

parser_test_sax_token_parser_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_sax_token_parser_CPPFLAGS = $(AM_CPPFLAGS)

# parser-test-threaded-json-parser

parser_test_threaded_json_parser_SOURCES = \
	threaded_json_parser_test.cpp

parser_test_threaded_json_parser_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_threaded_json_parser_LDFLAGS = -pthread
parser_test_threaded_json_parser_CPPFLAGS = $(AM_CPPFLAGS)

# parser-test-stream

parser_test_stream_SOURCES = \
	stream_test.cpp

parser_test_stream_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_stream_CPPFLAGS = $(AM_CPPFLAGS)

# parser-test-zip-archive

parser_test_zip_archive_SOURCES = \
	zip_archive_test.cpp

parser_test_zip_archive_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_zip_archive_CPPFLAGS = $(AM_CPPFLAGS)

TESTS = \
	parser-test-string-pool \
	parser-test-xml-namespace \
	parser-test-base64 \
	parser-test-sax-token-parser \
	parser-test-threaded-json-parser \
	parser-test-stream \
	parser-test-zip-archive

distclean-local:
	rm -rf $(TESTS)

