
INCLUDES = -I$(top_srcdir)/include

COMMON_CPPFLAGS = $(BOOST_SYSTEM_CFLAGS)

lib_LTLIBRARIES = liborcus-parser-@ORCUS_API_VERSION@.la
liborcus_parser_@ORCUS_API_VERSION@_la_SOURCES = \
	win_stdint.h \
	base64.cpp \
	cell_buffer.cpp \
	css_parser_base.cpp \
	css_types.cpp \
	csv_parser_base.cpp \
	exception.cpp \
	parser_global.cpp \
	pstring.cpp \
	sax_parser_base.cpp \
	stream.cpp \
	string_pool.cpp \
	types.cpp \
	xml_namespace.cpp \
	zip_archive.cpp \
	zip_archive_stream.cpp


liborcus_parser_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_parser_@ORCUS_API_VERSION@_la_LIBADD = \
						$(BOOST_SYSTEM_LIBS) \
						$(ZLIB_LIBS)

EXTRA_PROGRAMS = \
		parser-test-string-pool \
	parser-test-xml-namespace \
	parser-test-base64

# parser-test-string-pool

parser_test_string_pool_SOURCES = \
	string_pool.cpp \
	string_pool_test.cpp

parser_test_string_pool_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_string_pool_CPPFLAGS = $(COMMON_CPPFLAGS)

# parser-test-xml-namespace

parser_test_xml_namespace_SOURCES = \
	xml_namespace.cpp \
	xml_namespace_test.cpp

parser_test_xml_namespace_LDADD = liborcus-parser-@ORCUS_API_VERSION@.la
parser_test_xml_namespace_CPPFLAGS = $(COMMON_CPPFLAGS)

# parser-test-base64

parser_test_base64_SOURCES = \
	base64.cpp \
	base64_test.cpp

parser_test_base64_CPPFLAGS = $(COMMON_CPPFLAGS)

TESTS = \
	parser-test-string-pool \
	parser-test-xml-namespace \
	parser-test-base64

distclean-local:
	rm -rf $(TESTS)

