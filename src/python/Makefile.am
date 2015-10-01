if BUILD_PYTHON

pyexec_LTLIBRARIES = _orcus.la _orcus_json.la

_orcus_la_SOURCES = \
	python.cpp

_orcus_la_LDFLAGS = -module -avoid-version -export-symbols-regex PyInit__orcus
_orcus_la_CPPFLAGS = -I$(top_srcdir)/include $(PYTHON_CFLAGS) $(MDDS_CFLAGS)
_orcus_la_LIBADD = \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	$(PYTHON_LIBS)

_orcus_json_la_SOURCES = \
	json.cpp

_orcus_json_la_LDFLAGS = -module -avoid-version -export-symbols-regex PyInit__orcus_json
_orcus_json_la_CPPFLAGS = -I$(top_srcdir)/include $(PYTHON_CFLAGS) $(MDDS_CFLAGS)
_orcus_json_la_LIBADD = \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	$(PYTHON_LIBS)

orcusdir = $(pythondir)/orcus
orcus_DATA = \
	./orcus/__init__.py \
	./orcus/json.py

TESTS = \
	../../test/python/run-all.sh

endif
