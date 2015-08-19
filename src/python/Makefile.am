if BUILD_PYTHON

pyexec_LTLIBRARIES = orcus.la
orcus_la_SOURCES = \
	python.cpp

orcus_la_LDFLAGS = -module -avoid-version -export-symbols-regex initorcus
orcus_la_CPPFLAGS = -I$(top_srcdir)/include $(PYTHON_CFLAGS) $(MDDS_CFLAGS)
orcus_la_LIBADD = \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la \
	$(PYTHON_LIBS)

AM_TESTS_ENVIRONMENT = PYTHONPATH=.libs$${PYTHONPATH:+:$${PYTHONPATH}}; export PYTHONPATH;
TESTS = \
	../../test/python/module.py

endif
