SUBDIRS = parser mso liborcus spreadsheet

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS =

EXTRA_PROGRAMS = \
	orcus-test-common \
	orcus-test-xml

COMMON_CPPFLAGS = $(BOOST_SYSTEM_CFLAGS)

if HAVE_STATIC_LIB
COMMON_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
endif

# orcus-test-common

orcus_test_common_SOURCES = orcus_test_common.cpp
orcus_test_common_LDADD = \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_test_common_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-test-xml

orcus_test_xml_SOURCES = orcus_test_xml.cpp
orcus_test_xml_LDADD = \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_test_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS) -DSRCDIR=\""$(top_srcdir)"\"


TESTS = \
	orcus-test-common \
	orcus-test-xml

if WITH_TOOLS

bin_PROGRAMS += \
	orcus-zip-dump \
	orcus-xml-dump \
	orcus-mso-encryption

# orcus-zip-dump

orcus_zip_dump_SOURCES = \
	orcus_zip_dump.cpp

orcus_zip_dump_LDADD = \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la

orcus_zip_dump_CPPFLAGS =

# orcus-xml-dump

orcus_xml_dump_SOURCES = \
	orcus_xml_dump.cpp \
	dom_tree_sax_handler.hpp \
	dom_tree_sax_handler.cpp

orcus_xml_dump_LDADD = \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	liborcus/liborcus-@ORCUS_API_VERSION@.la

orcus_xml_dump_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

#----------------------------------------------------------------------------

# orcus-mso-encryption

orcus_mso_encryption_SOURCES = orcus_mso_encryption.cpp
orcus_mso_encryption_LDADD = \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	mso/liborcus-mso-@ORCUS_API_VERSION@.la \
	$(BOOST_SYSTEM_LIB)

orcus_mso_encryption_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

#----------------------------------------------------------------------------

if BUILD_SPREADSHEET_MODEL

bin_PROGRAMS += \
	orcus-csv orcus-xml

# orcus-csv

orcus_csv_SOURCES = \
	orcus_csv_main.cpp

orcus_csv_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la
orcus_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

# orcus-xml

orcus_xml_SOURCES = \
	xml_map_sax_handler.cpp \
	orcus_xml_main.cpp

orcus_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

if WITH_ODS_FILTER

bin_PROGRAMS += \
	orcus-ods

# orcus-ods

orcus_ods_SOURCES = \
	orcus_ods_main.cpp

orcus_ods_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_ods_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

endif # WITH_ODS_FILTER

if WITH_XLSX_FILTER

bin_PROGRAMS += \
	orcus-xlsx

# orcus-xlsx

orcus_xlsx_SOURCES = \
	orcus_xlsx_main.cpp

orcus_xlsx_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_xlsx_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

endif # WITH_XLSX_FILTER

if WITH_XLS_XML_FILTER

bin_PROGRAMS += \
	orcus-xls-xml

# orcus-xls-xml

orcus_xls_xml_SOURCES = \
	orcus_xls_xml_main.cpp

orcus_xls_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_xls_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

endif # WITH_XLS_XML_FILTER

if WITH_GNUMERIC_FILTER

bin_PROGRAMS += \
	orcus-gnumeric

# orcus-gnumeric

orcus_gnumeric_SOURCES = \
	orcus_gnumeric_main.cpp

orcus_gnumeric_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_gnumeric_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS)

endif # WITH_GNUMERIC_FILTER

endif # BUILD_SPREADSHEET_MODEL

endif # WITH_TOOLS

#----------------------------------------------------------------------------
# Orcus Filter Tests
#----------------------------------------------------------------------------

if BUILD_SPREADSHEET_MODEL

EXTRA_PROGRAMS += \
	orcus-test-csv \
	orcus-test-xml-mapped

# orcus-test-csv

orcus_test_csv_SOURCES = orcus_test_csv.cpp
orcus_test_csv_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_csv_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS) -DSRCDIR=\""$(top_srcdir)"\"

# orcus-test-xml-mapped

orcus_test_xml_mapped_SOURCES = \
	xml_map_sax_handler.cpp \
	xml_map_sax_handler.hpp \
	dom_tree_sax_handler.cpp \
	dom_tree_sax_handler.hpp \
	orcus_test_xml_mapped.cpp

orcus_test_xml_mapped_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_xml_mapped_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la $(COMMON_CPPFLAGS) -DSRCDIR=\""$(top_srcdir)"\"

TESTS += \
	orcus-test-csv \
	orcus-test-xml-mapped

if WITH_ODS_FILTER

EXTRA_PROGRAMS += \
	orcus-test-ods

# orcus-test-ods

orcus_test_ods_SOURCES = \
	orcus_test_ods.cpp

orcus_test_ods_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_ods_CPPFLAGS = \
	-I$(top_builddir)/lib/liborcus/liborcus.la -DSRCDIR=\""$(top_srcdir)"\" \
	 $(MDDS_CFLAGS)

TESTS += orcus-test-ods

endif # WITH_ODS_FILTER

if WITH_XLSX_FILTER

EXTRA_PROGRAMS += \
	orcus-test-xlsx

# orcus-test-xlsx

orcus_test_xlsx_SOURCES = \
	orcus_test_xlsx.cpp

orcus_test_xlsx_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_xlsx_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la -DSRCDIR=\""$(top_srcdir)"\"

TESTS += \
	 orcus-test-xlsx

endif # WITH_XLSX_FILTER

if WITH_XLS_XML_FILTER

EXTRA_PROGRAMS += orcus-test-xls-xml

# orcus-test-xls-xml

orcus_test_xls_xml_SOURCES = \
	orcus_test_xls_xml.cpp

orcus_test_xls_xml_LDADD = \
	liborcus/liborcus-@ORCUS_API_VERSION@.la \
	parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

orcus_test_xls_xml_CPPFLAGS = -I$(top_builddir)/lib/liborcus/liborcus.la -DSRCDIR=\""$(top_srcdir)"\"

TESTS += \
	 orcus-test-xls-xml

endif # WITH_XLS_XML_FILTER

endif # BUILD_SPREADSHEET_MODEL

distclean-local:
	rm -rf $(TESTS)

