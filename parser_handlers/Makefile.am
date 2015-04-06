SUBDIRS = .

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS =

EXTRA_PROGRAMS = \
	orcus-handler-test-css \
	orcus-handler-test-csv \
	orcus-handler-test-sax

COMMON_CPPFLAGS =

# orcus-handler-test-css

orcus_handler_test_css_SOURCES = css_test.cpp
orcus_handler_test_css_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_handler_test_css_CPPFLAGS = $(COMMON_CPPFLAGS)

# orcus-handler-test-csv

orcus_handler_test_csv_SOURCES = csv_test.cpp
orcus_handler_test_csv_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_handler_test_csv_CPPFLAGS = $(COMMON_CPPFLAGS)

# orcus-handler-test-sax

orcus_handler_test_sax_SOURCES = sax_test.cpp
orcus_handler_test_sax_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

TESTS = \
	orcus-handler-test-css \
	orcus-handler-test-csv \
	orcus-handler-test-sax

distclean-local:
	rm -rf $(TESTS)

