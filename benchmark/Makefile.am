
AM_CPPFLAGS = -I$(top_srcdir)/include

EXTRA_PROGRAMS = \
	json-parser-test \
	threaded-json-parser-test

json_parser_test_SOURCES = \
	json_parser.cpp

json_parser_test_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

json_parser_test_CPPFLAGS = $(AM_CPPFLAGS)


threaded_json_parser_test_SOURCES = \
	threaded_json_parser.cpp

threaded_json_parser_test_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

threaded_json_parser_test_CPPFLAGS = $(AM_CPPFLAGS)

CLEANFILES = $(EXTRA_PROGRAMS)

.PHONY: all

all: $(EXTRA_PROGRAMS)

