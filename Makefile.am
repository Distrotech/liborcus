SUBDIRS = src include
ACLOCAL_AMFLAGS = -I m4

pcfiles = liborcus-@ORCUS_API_VERSION@.pc
liborcus-@ORCUS_API_VERSION@.pc: liborcus.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

doc_data = \
	doc/class_diagram.odg \
	doc/rfc4180-csv.txt

bin_data = \
	bin/gen-odf-tokens.py \
	bin/gen-ooxml-tokens.py \
	bin/orcus-common-func.sh \
	bin/orcus-csv.sh \
	bin/orcus-ods.sh \
	bin/orcus-test-xml.sh \
	bin/orcus-xlsx.sh \
	bin/orcus-xml.sh \
	bin/token_util.py \
	bin/token_util.pyc \
	bin/orcus-xml-dump.sh \
	bin/pack-release.sh

misc_data = \
	misc/OpenDocument-schema-v1.2-cd04.rng \
	misc/libzip_test.cpp \
	misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip \
	misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip \
	misc/generate-tokens.sh

slickedit_data = \
	slickedit/orcus.vpw \
	slickedit/token-gen.vpj \
	slickedit/orcus.vpj

test_data = \
	test/css/test-complex.css \
	test/css/test.css \
	test/csv/double-quotes.csv \
	test/csv/normal-quotes.csv \
	test/csv/simple-numbers.csv \
	test/csv/single-quoted-cell.csv \
	test/ods/formula.ods \
	test/ods/japanese.ods \
	test/ods/test.ods \
	test/xlsx/formula-shared.xlsx \
	test/xlsx/formula-simple.xlsx \
	test/xlsx/test.xlsx \
	test/xml-mapped/basic-map.xml \
	test/xml-mapped/basic.check \
	test/xml-mapped/basic.xml \
	test/xml/simple.xml \
	test/xml/simple.xml.check \
	test-manual/ods/cell-styles-ru.ods \
	test-manual/ods/george-ou-perf.ods

vsprojects_data = \
	vsprojects/liborcus-static-nozip/liborcus-static-nozip.vcproj \
	vsprojects/liborcus/DefaultConfig.vsprops \
	vsprojects/liborcus/LibZip.vsprops \
	vsprojects/liborcus/liborcus.vcproj \
	vsprojects/liborcus-spreadsheet-model/liborcus-spreadsheet-model.vcproj \
	vsprojects/orcus-csv/AdditionalLibs.vsprops \
	vsprojects/orcus-csv/orcus-csv.vcproj \
	vsprojects/orcus-ods/orcus-ods.vcproj \
	vsprojects/orcus-xlsx/orcus-xlsx.vcproj \
	vsprojects/orcus-xml/orcus-xml.vcproj \
	vsprojects/orcus.sln \
	vsprojects/MddsAndIxion.vsprops

EXTRA_DIST = \
	liborcus.pc.in \
	autogen.sh \
	$(bin_data) \
	$(doc_data) \
	$(misc_data) \
	$(slickedit_data) \
	$(test_data) \
	$(vsprojects_data)


dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog
