
AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LTLIBRARIES = liborcus-mso-@ORCUS_API_VERSION@.la
liborcus_mso_@ORCUS_API_VERSION@_la_SOURCES = \
	encryption_info.cpp

liborcus_mso_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_mso_@ORCUS_API_VERSION@_la_LIBADD = \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la
