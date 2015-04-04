SUBDIRS = .

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS =

EXTRA_PROGRAMS = \
	orcus-handler-test-css

COMMON_CPPFLAGS =

# orcus-handler-test-css

orcus_handler_test_css_SOURCES = css_test.cpp
orcus_handler_test_css_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_handler_test_css_CPPFLAGS = $(COMMON_CPPFLAGS)

TESTS = \
	orcus-handler-test-css

distclean-local:
	rm -rf $(TESTS)

