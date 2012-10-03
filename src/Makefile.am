SUBDIRS = liborcus spreadsheet

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS = orcus-xml-dump

EXTRA_PROGRAMS = \
	orcus-test-common \
	orcus-test-xml

COMMON_CPPFLAGS =

if HAVE_STATIC_LIB
COMMON_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
endif

# orcus-xml-dump

orcus_xml_dump_SOURCES = \
	orcus_xml_dump.cpp \
	dom_tree_sax_handler.hpp \
	dom_tree_sax_handler.cpp

orcus_xml_dump_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_xml_dump_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-test-common

orcus_test_common_SOURCES = orcus_test_common.cpp
orcus_test_common_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_test_common_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-test-xml

orcus_test_xml_SOURCES = orcus_test_xml.cpp
orcus_test_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_test_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)


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
orcus_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-xml

orcus_xml_SOURCES = \
	xml_map_sax_handler.cpp \
	orcus_xml_main.cpp

orcus_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-test-csv

orcus_test_csv_SOURCES = orcus_test_csv.cpp
orcus_test_csv_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-test-xml-mapped

orcus_test_xml_mapped_SOURCES = \
	xml_map_sax_handler.cpp \
	xml_map_sax_handler.hpp \
	dom_tree_sax_handler.cpp \
	dom_tree_sax_handler.hpp \
	orcus_test_xml_mapped.cpp

orcus_test_xml_mapped_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_xml_mapped_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

TESTS += \
	orcus-test-csv \
	orcus-test-xml-mapped

if WITH_LIBZIP

if WITH_ODS_FILTER

bin_PROGRAMS += \
	orcus-ods

endif #WITH_ODS_FILTER

if WITH_XLSX_FILTER

bin_PROGRAMS += \
	orcus-xlsx

endif #WITH_XLSX_FILTER

if WITH_GNUMERIC_FILTER

bin_PROGRAMS += \
	orcus-gnumeric

endif #WITH_GNUMERIC_FILTER

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

# orcus_gnumeric

orcus_gnumeric_SOURCES = \
	orcus_gnumeric_main.cpp

orcus_gnumeric_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_gnumeric_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la

if HAVE_STATIC_LIB
orcus_ods_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
orcus_xlsx_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
orcus_gnumeric_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
endif

endif # WITH_LIBZIP

endif # BUILD_SPREADSHEET_MODEL

