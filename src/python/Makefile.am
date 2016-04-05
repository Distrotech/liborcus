if BUILD_PYTHON

AM_CPPFLAGS = $(BOOST_CPPFLAGS) $(PYTHON_CFLAGS) $(MDDS_CFLAGS) $(LIBIXION_CFLAGS)

pyexec_LTLIBRARIES = _orcus.la _orcus_json.la

_orcus_la_SOURCES = \
	python.cpp \
	root.hpp \
	root.cpp \
	xlsx.hpp \
	xlsx.cpp

_orcus_la_LDFLAGS = -module -avoid-version -export-symbols-regex PyInit__orcus
_orcus_la_CPPFLAGS = -I$(top_srcdir)/include $(AM_CPPFLAGS)
_orcus_la_LIBADD = \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	$(PYTHON_LIBS)

if BUILD_SPREADSHEET_MODEL

_orcus_la_SOURCES += \
	document.hpp \
	document.cpp \
	sheet.hpp \
	sheet.cpp \
	sheet_rows.hpp \
	sheet_rows.cpp

_orcus_la_LIBADD += \
	../spreadsheet/liborcus-spreadsheet-model-@ORCUS_API_VERSION@.la

endif # BUILD_SPREADSHEET_MODEL

_orcus_json_la_SOURCES = \
	json.cpp

_orcus_json_la_LDFLAGS = -module -avoid-version -export-symbols-regex PyInit__orcus_json
_orcus_json_la_CPPFLAGS = -I$(top_srcdir)/include $(AM_CPPFLAGS)
_orcus_json_la_LIBADD = \
	../liborcus/liborcus-@ORCUS_API_VERSION@.la \
	../parser/liborcus-parser-@ORCUS_API_VERSION@.la \
	$(PYTHON_LIBS)

orcusdir = $(pythondir)/orcus
orcus_DATA = \
	./orcus/__init__.py \
	./orcus/json.py

EXTRA_DIST = \
	./orcus/__init__.py \
	./orcus/json.py

AM_TESTS_ENVIRONMENT = PYTHONPATH=$(top_srcdir)/src/python:.libs$${PYTHONPATH:+:$${PYTHONPATH}}; export PYTHONPATH;

TESTS = \
	../../test/python/json.py \
	../../test/python/module.py \
	../../test/python/xlsx.py

endif # BUILD_PYTHON
