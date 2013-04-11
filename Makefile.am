SUBDIRS = src include
ACLOCAL_AMFLAGS = -I m4

pcfiles = liborcus-@ORCUS_API_VERSION@.pc
if BUILD_SPREADSHEET_MODEL
pcfiles += liborcus-spreadsheet-model-@ORCUS_API_VERSION@.pc
endif

liborcus-@ORCUS_API_VERSION@.pc: liborcus.pc
	@cp -f $< $@

liborcus-spreadsheet-model-@ORCUS_API_VERSION@.pc: liborcus-spreadsheet-model.pc
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
	misc/OpenDocument-v1.2-os-schema.rng \
	misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip \
	misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip \
	misc/gnumeric.xsd \
	misc/generate-tokens.sh

slickedit_data = \
	slickedit/orcus.vpw \
	slickedit/token-gen.vpj \
	slickedit/orcus.vpj

test_data = \
	test/xml/simple/check.txt \
	test/xml/simple/input.xml \
	test/xml/encoded-char/check.txt \
	test/xml/encoded-char/input.xml \
	test/xml/default-ns/check.txt \
	test/xml/default-ns/input.xml \
	test/xml/ns-alias-1/check.txt \
	test/xml/ns-alias-1/input.xml \
	test/test-nocolor.xlsx \
	test/gnumeric/test.gnumeric \
	test/css/test-complex.css \
	test/css/test.css \
	test/ods/formula.ods \
	test/ods/test.ods \
	test/ods/japanese.ods \
	test/xml-mapped/content-namespace/map.xml \
	test/xml-mapped/content-namespace/check.txt \
	test/xml-mapped/content-namespace/input.xml \
	test/xml-mapped/content-namespace-2/map.xml \
	test/xml-mapped/content-namespace-2/check.txt \
	test/xml-mapped/content-namespace-2/input.xml \
	test/xml-mapped/content-basic/map.xml \
	test/xml-mapped/content-basic/check.txt \
	test/xml-mapped/content-basic/input.xml \
	test/xml-mapped/attribute-single-element-2/map.xml \
	test/xml-mapped/attribute-single-element-2/check.txt \
	test/xml-mapped/attribute-single-element-2/input.xml \
	test/xml-mapped/attribute-single-element/map.xml \
	test/xml-mapped/attribute-single-element/check.txt \
	test/xml-mapped/attribute-single-element/input.xml \
	test/xml-mapped/attribute-namespace/map.xml \
	test/xml-mapped/attribute-namespace/check.txt \
	test/xml-mapped/attribute-namespace/input.xml \
	test/xml-mapped/attribute-range-self-close/map.xml \
	test/xml-mapped/attribute-range-self-close/check.txt \
	test/xml-mapped/attribute-range-self-close/input.xml \
	test/xml-mapped/attribute-basic/map.xml \
	test/xml-mapped/attribute-basic/check.txt \
	test/xml-mapped/attribute-basic/input.xml \
	test/csv/quoted-with-space.csv \
	test/csv/double-quotes.csv \
	test/csv/simple-numbers.csv \
	test/csv/single-quoted-cell.csv \
	test/csv/normal-quotes.csv \
	test/xml-structure/basic-1/check.txt \
	test/xml-structure/basic-1/input.xml \
	test/xml-structure/basic-2/check.txt \
	test/xml-structure/basic-2/input.xml \
	test/xml-structure/attribute-1/check.txt \
	test/xml-structure/attribute-1/input.xml \
	test/xml-structure/basic-3/check.txt \
	test/xml-structure/basic-3/input.xml \
	test/xml-structure/nested-repeat-1/check.txt \
	test/xml-structure/nested-repeat-1/input.xml \
	test/xml-structure/namespace-default/check.txt \
	test/xml-structure/namespace-default/input.xml \
	test/xlsx/formula-simple.xlsx \
	test/xlsx/test.xlsx \
	test/xlsx/formula-on-sheet3.xlsx \
	test/xlsx/formula-shared.xlsx

vsprojects_data = \
	vsprojects/MddsAndIxion.vsprops \
	vsprojects/liborcus-parser-static/liborcus-parser-static.vcproj \
	vsprojects/liborcus-parser.vsprops \
	vsprojects/liborcus-parser/liborcus-parser.vcproj \
	vsprojects/liborcus-spreadsheet-model/liborcus-spreadsheet-model.vcproj \
	vsprojects/liborcus-static/liborcus-static.vcproj \
	vsprojects/liborcus/DefaultConfig.vsprops \
	vsprojects/liborcus/liborcus.vcproj \
	vsprojects/liborcus/zLib.vsprops \
	vsprojects/orcus-csv/AdditionalLibs.vsprops \
	vsprojects/orcus-csv/orcus-csv.vcproj \
	vsprojects/orcus-gnumeric/orcus-gnumeric.vcproj \
	vsprojects/orcus-ods/orcus-ods.vcproj \
	vsprojects/orcus-xlsx/orcus-xlsx.vcproj \
	vsprojects/orcus-xml/orcus-xml.vcproj

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
