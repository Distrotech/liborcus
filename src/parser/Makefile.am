
INCLUDES = -I$(top_srcdir)/include

COMMON_CPPFLAGS = $(BOOST_SYSTEM_CFLAGS)

lib_LTLIBRARIES = liborcus-parser-@ORCUS_API_VERSION@.la
liborcus_parser_@ORCUS_API_VERSION@_la_SOURCES = \
	base64.cpp \
	cell_buffer.cpp \
	exception.cpp \
	sax_parser_global.cpp \
	stream.cpp

liborcus_parser_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined

EXTRA_PROGRAMS = \
	parser-test-base64

# parser-test-base64

parser_test_base64_SOURCES = \
	base64.hpp \
	base64.cpp \
	base64_test.cpp

parser_test_base64_CPPFLAGS = $(COMMON_CPPFLAGS)

TESTS = \
	parser-test-base64
