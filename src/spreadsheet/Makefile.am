if BUILD_SPREADSHEET_MODEL

INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_SOURCES = \
	document.cpp \
	document.hpp \
	factory.cpp \
	factory.hpp \
	shared_strings.cpp \
	shared_strings.hpp \
	sheet.cpp \
	sheet.hpp \
	styles.cpp \
	styles.hpp \
	global_settings.cpp \
	global_settings.hpp

liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_CXXFLAGS = $(LIBIXION_CFLAGS)
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_spreadsheet_model_@ORCUS_API_VERSION@_la_LIBADD = $(LIBIXION_LIBS)

endif
