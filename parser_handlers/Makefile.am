SUBDIRS = .

AM_CPPFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS =

EXTRA_PROGRAMS = \
	orcus-handler-test-css \
	orcus-handler-test-csv \
	orcus-handler-test-sax \
	orcus-handler-test-sax-token \
	orcus-handler-test-sax-ns \
	orcus-handler-test-json

# orcus-handler-test-css

orcus_handler_test_css_SOURCES = \
	css.hpp \
	css_test.cpp

orcus_handler_test_css_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_handler_test_css_CPPFLAGS = $(AM_CPPFLAGS)

# orcus-handler-test-csv

orcus_handler_test_csv_SOURCES = \
	csv.hpp \
	csv_test.cpp

orcus_handler_test_csv_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_handler_test_csv_CPPFLAGS = $(AM_CPPFLAGS)

# orcus-handler-test-sax

orcus_handler_test_sax_SOURCES = \
	sax.hpp \
	sax_test.cpp

orcus_handler_test_sax_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

# orcus-handler-test-sax-token

orcus_handler_test_sax_token_SOURCES = \
	sax_token.hpp \
	sax_token_test.cpp

orcus_handler_test_sax_token_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

# orcus-handler-test-sax-ns

orcus_handler_test_sax_ns_SOURCES = \
	sax_ns.hpp \
	sax_ns_test.cpp

orcus_handler_test_sax_ns_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

# orcus-handler-test-json

orcus_handler_test_json_SOURCES = \
	json.hpp \
	json_test.cpp

orcus_handler_test_json_LDADD = \
	../src/parser/liborcus-parser-@ORCUS_API_VERSION@.la

TESTS = \
	orcus-handler-test-css \
	orcus-handler-test-csv \
	orcus-handler-test-sax \
	orcus-handler-test-sax-token \
	orcus-handler-test-sax-ns \
	orcus-handler-test-json


distclean-local:
	rm -rf $(TESTS)

