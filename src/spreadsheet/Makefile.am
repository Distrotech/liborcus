if BUILD_SPREADSHEET_MODEL

INCLUDES = -I$(top_srcdir)/include

COMMON_CPPFLAGS =

if HAVE_STATIC_LIB
COMMON_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
endif

lib_LTLIBRARIES = liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_SOURCES = \
	data_table.cpp \
	document.cpp \
	factory.cpp \
	shared_strings.cpp \
	sheet.cpp \
	sheet_properties.cpp \
	styles.cpp \
	global_settings.cpp

liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_CPPFLAGS = $(COMMON_CPPFLAGS)
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_CXXFLAGS = $(LIBIXION_CFLAGS) $(MDDS_CFLAGS)
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LIBADD = $(LIBIXION_LIBS) \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la

endif
