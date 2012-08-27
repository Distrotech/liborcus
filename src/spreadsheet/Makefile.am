if BUILD_SPREADSHEET

INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = liborcus-spreadsheet-impl-@ORCUS_API_VERSION@.la
liborcus_spreadsheet_impl_@ORCUS_API_VERSION@_la_SOURCES = \
	document.cpp \
	import_factory.cpp \
	shared_strings.cpp \
	sheet.cpp \
	styles.cpp

liborcus_spreadsheet_impl_@ORCUS_API_VERSION@_la_CPPFLAGS = `@PKG_CONFIG_CPPFLAGS_SPREADSHEET@`
liborcus_spreadsheet_impl_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined `@PKG_CONFIG_LDFLAGS_SPREADSHEET@`
liborcus_spreadsheet_impl_@ORCUS_API_VERSION@_la_LIBADD =

endif
