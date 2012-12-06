
INCLUDES = -I$(top_srcdir)/include

lib_LTLIBRARIES = liborcus-parser-@ORCUS_API_VERSION@.la
liborcus_parser_@ORCUS_API_VERSION@_la_SOURCES = \
	cell_buffer.cpp

liborcus_parser_@ORCUS_API_VERSION@_la_CXXFLAGS = $(LIBIXION_CFLAGS)
liborcus_parser_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined
liborcus_parser_@ORCUS_API_VERSION@_la_LIBADD = $(LIBIXION_LIBS)
