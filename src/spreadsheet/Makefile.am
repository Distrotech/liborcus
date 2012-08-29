if BUILD_SPREADSHEET_MODEL

INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_SOURCES = \
	document.cpp \
	factory.cpp \
	shared_strings.cpp \
	sheet.cpp \
	styles.cpp

liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_CPPFLAGS = `@PKG_CONFIG_CPPFLAGS_SPREADSHEET_MODEL@`
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined `@PKG_CONFIG_LDFLAGS_SPREADSHEET_MODEL@`
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LIBADD =

endif
