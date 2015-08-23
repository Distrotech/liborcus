if BUILD_PYTHON

pyexec_LTLIBRARIES = orcus.la
orcus_la_SOURCES = \
	python.cpp

orcus_la_LDFLAGS = -module -avoid-version -export-symbols-regex PyInit_orcus
orcus_la_CPPFLAGS = -I$(top_srcdir)/include $(PYTHON_CFLAGS) $(MDDS_CFLAGS)
orcus_la_LIBADD = \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	$(PYTHON_LIBS)

TESTS = \
	../../test/python/run-all.sh

endif
