SUBDIRS = liborcus spreadsheet

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = orcus-format-xml

EXTRA_PROGRAMS = \
	orcus-test-common \
	orcus-test-xml

# orcus-format-xml

orcus_format_xml_SOURCES = orcus_format_xml.cpp
orcus_format_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_format_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

# orcus-test-common

orcus_test_common_SOURCES = orcus_test_common.cpp
orcus_test_common_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_test_common_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

# orcus-test-xml

orcus_test_xml_SOURCES = orcus_test_xml.cpp
orcus_test_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_test_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

TESTS = \
	orcus-test-common \
	orcus-test-xml

#----------------------------------------------------------------------------
# spreadsheet model
#----------------------------------------------------------------------------

if BUILD_SPREADSHEET_MODEL

bin_PROGRAMS += \
	orcus-csv orcus-xml

EXTRA_PROGRAMS += \
	orcus-test-csv \
	orcus-test-xml-mapped

# orcus-csv

orcus_csv_SOURCES = \
	orcus_csv_main.cpp

orcus_csv_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la
orcus_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

# orcus-xml

orcus_xml_SOURCES = \
	xml_map_sax_handler.cpp \
	orcus_xml_main.cpp

orcus_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

# orcus-test-csv

orcus_test_csv_SOURCES = orcus_test_csv.cpp
orcus_test_csv_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

# orcus-test-xml-mapped

orcus_test_xml_mapped_SOURCES = \
	xml_map_sax_handler.cpp \
	orcus_test_xml_mapped.cpp

orcus_test_xml_mapped_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_xml_mapped_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

TESTS += \
	orcus-test-csv \
	orcus-test-xml-mapped

if WITH_LIBZIP

bin_PROGRAMS += \
	orcus-ods orcus-xlsx

# orcus-ods

orcus_ods_SOURCES = \
	orcus_ods_main.cpp

orcus_ods_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la \
	$(LIBZIP_LIBS)

orcus_ods_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

# orcus-xlsx

orcus_xlsx_SOURCES = \
	orcus_xlsx_main.cpp

orcus_xlsx_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_xlsx_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

endif # WITH_LIBZIP

endif # BUILD_SPREADSHEET_MODEL

