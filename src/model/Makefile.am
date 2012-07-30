
INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = liborcus-model-impl-@ORCUS_API_VERSION@.la
liborcus_model_impl_@ORCUS_API_VERSION@_la_SOURCES = \
	document.cpp \
	factory.cpp \
	shared_strings.cpp \
	sheet.cpp \
	styles.cpp

liborcus_model_impl_@ORCUS_API_VERSION@_la_CPPFLAGS = `@PKG_CONFIG_CPPFLAGS@`
liborcus_model_impl_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined `@PKG_CONFIG_LDFLAGS@`
liborcus_model_impl_@ORCUS_API_VERSION@_la_LIBADD =

