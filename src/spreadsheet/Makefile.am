if BUILD_SPREADSHEET_MODEL

AM_CPPFLAGS = -I$(top_srcdir)/include -D__ORCUS_SPM_BUILDING_DLL

AM_CPPFLAGS += $(BOOST_CPPFLAGS) $(LIBIXION_CFLAGS)

COMMON_CPPFLAGS = $(AM_CPPFLAGS)

if HAVE_STATIC_LIB
AM_CPPFLAGS += -D__ORCUS_STATIC_LIB=1
endif

lib_LTLIBRARIES = liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_SOURCES = \
	auto_filter.cpp \
	data_table.hpp \
	data_table.cpp \
	document.cpp \
	factory.cpp \
	formula_global.hpp \
	formula_global.cpp \
	shared_strings.cpp \
	sheet.cpp \
	sheet_properties.cpp \
	sheet_range.cpp \
	styles.cpp \
	table.hpp \
	table.cpp \
	global_settings.cpp

liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_CPPFLAGS = $(AM_CPPFLAGS) $(LIBIXION_CFLAGS)
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LIBADD = $(LIBIXION_LIBS) \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la

endif
