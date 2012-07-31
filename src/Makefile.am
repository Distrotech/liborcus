SUBDIRS = liborcus model .

if BUILD_MODEL

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = orcus-csv orcus-ods orcus-xlsx \
	orcus-csv-test

orcus_csv_SOURCES = \
	orcus_csv_main.cpp

orcus_csv_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	model/liborcus-model-impl-@ORCUS_API_VERSION@.la

orcus_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

orcus_ods_SOURCES = \
	orcus_ods_main.cpp

orcus_ods_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	model/liborcus-model-impl-@ORCUS_API_VERSION@.la

orcus_ods_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

orcus_xlsx_SOURCES = \
	orcus_xlsx_main.cpp

orcus_xlsx_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	model/liborcus-model-impl-@ORCUS_API_VERSION@.la

orcus_xlsx_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

orcus_csv_test_SOURCES = orcus_csv_test.cpp
orcus_csv_test_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	model/liborcus-model-impl-@ORCUS_API_VERSION@.la

orcus_csv_test_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

TESTS = orcus-csv-test

endif
