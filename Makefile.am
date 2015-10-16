SUBDIRS = src include parser_handlers
ACLOCAL_AMFLAGS = -I m4

pcfiles = liborcus-@ORCUS_API_VERSION@.pc
if BUILD_SPREADSHEET_MODEL
pcfiles += liborcus-spreadsheet-model-@ORCUS_API_VERSION@.pc
endif

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

doc_data = \
    doc/rfc4180-csv.txt \
    doc/class_diagram.odg \
    doc/column-width-and-row-height.txt \
    doc/zlib-in-memory-gzip.txt

bin_data = \
    bin/orcus-xls-xml.sh \
    bin/orcus-detect.sh \
    bin/orcus-test-csv.sh \
    bin/orcus-xml.sh \
    bin/orcus-xml-dump.sh \
    bin/orcus-csv.sh \
    bin/orcus-test-xml-mapped.sh \
    bin/orcus-mso-encryption.sh \
    bin/orcus-xlsx.sh \
    bin/orcus-ods.sh \
    bin/orcus-gnumeric.sh \
    bin/orcus-zip-dump.sh \
    bin/orcus-common-func.sh \
    bin/orcus-test-xml.sh

misc_data = \
    misc/OpenDocument-v1.2-os-schema.rng \
    misc/gnumeric.xsd \
    misc/xls-xml-tokens.txt \
    misc/gen-odf-tokens.py \
    misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip \
    misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip \
    misc/gen-gnumeric-tokens.py \
    misc/gen-tokens.py \
    misc/gen-ooxml-tokens.py \
    misc/generate-tokens.sh \
    misc/dump-xsd-keys.py \
    misc/token_util.py

slickedit_data = \
    slickedit/token-gen.vpj \
    slickedit/orcus.vpw \
    slickedit/external-headers.vpj \
    slickedit/orcus.vpj

test_data = \
    test/csv/normal-quotes/input.csv \
    test/csv/normal-quotes/check.txt \
    test/csv/quoted-with-delim/input.csv \
    test/csv/quoted-with-delim/check.txt \
    test/csv/simple-numbers/input.csv \
    test/csv/simple-numbers/check.txt \
    test/csv/double-quotes/input.csv \
    test/csv/double-quotes/check.txt \
    test/xml-structure/basic-1/check.txt \
    test/xml-structure/basic-1/input.xml \
    test/xml-structure/attribute-1/check.txt \
    test/xml-structure/attribute-1/input.xml \
    test/xml-structure/namespace-default/check.txt \
    test/xml-structure/namespace-default/input.xml \
    test/xml-structure/basic-2/check.txt \
    test/xml-structure/basic-2/input.xml \
    test/xml-structure/nested-repeat-1/check.txt \
    test/xml-structure/nested-repeat-1/input.xml \
    test/xml-structure/basic-3/check.txt \
    test/xml-structure/basic-3/input.xml \
    test/xls-xml/basic/check.txt \
    test/xls-xml/basic/input.xml \
    test/xml-mapped/attribute-single-element-2/check.txt \
    test/xml-mapped/attribute-single-element-2/map.xml \
    test/xml-mapped/attribute-single-element-2/input.xml \
    test/xml-mapped/attribute-single-element/check.txt \
    test/xml-mapped/attribute-single-element/map.xml \
    test/xml-mapped/attribute-single-element/input.xml \
    test/xml-mapped/attribute-range-self-close/check.txt \
    test/xml-mapped/attribute-range-self-close/map.xml \
    test/xml-mapped/attribute-range-self-close/input.xml \
    test/xml-mapped/attribute-namespace/check.txt \
    test/xml-mapped/attribute-namespace/map.xml \
    test/xml-mapped/attribute-namespace/input.xml \
    test/xml-mapped/attribute-basic/check.txt \
    test/xml-mapped/attribute-basic/map.xml \
    test/xml-mapped/attribute-basic/input.xml \
    test/xml-mapped/content-basic/check.txt \
    test/xml-mapped/content-basic/map.xml \
    test/xml-mapped/content-basic/input.xml \
    test/xml-mapped/content-namespace-2/check.txt \
    test/xml-mapped/content-namespace-2/map.xml \
    test/xml-mapped/content-namespace-2/input.xml \
    test/xml-mapped/content-namespace/check.txt \
    test/xml-mapped/content-namespace/map.xml \
    test/xml-mapped/content-namespace/input.xml \
    test/gnumeric/test.gnumeric \
    test/xlsx/test.xlsx \
    test/xlsx/column-width-row-height/input.xlsx \
    test/xlsx/empty-shared-strings/check.txt \
    test/xlsx/empty-shared-strings/input.xlsx \
    test/xlsx/data-table/multi-table.xlsx \
    test/xlsx/data-table/one-variable.xlsx \
    test/xlsx/borders/single-cells.xlsx \
    test/xlsx/borders/grid-box.xlsx \
    test/xlsx/formula-simple.xlsx \
    test/xlsx/raw-values-1/check.txt \
    test/xlsx/raw-values-1/input.xlsx \
    test/xlsx/pivot-table/two-pivot-caches.xlsx \
    test/xlsx/pivot-table/two-tables-one-source.xlsx \
    test/xlsx/pivot-table/many-fields.xlsx \
    test/xlsx/date-cell/input.xlsx \
    test/xlsx/table/table-2.xlsx \
    test/xlsx/table/autofilter.xlsx \
    test/xlsx/table/table-1.xlsx \
    test/xlsx/formatted-text/colored-text.xlsx \
    test/xlsx/formatted-text/bold-and-italic.xlsx \
    test/xlsx/revision/cell-change-basic.xlsx \
    test/xlsx/background-color/standard.xlsx \
    test/xlsx/formula-shared.xlsx \
    test/css/basic5.css \
    test/css/basic11.css \
    test/css/complex/excel-html.css \
    test/css/complex/callout.css \
    test/css/basic12.css \
    test/css/test.css \
    test/css/basic13.css \
    test/css/basic4.css \
    test/css/basic8.css \
    test/css/basic1.css \
    test/css/basic9.css \
    test/css/basic10.css \
    test/css/chained2.css \
    test/css/basic2.css \
    test/css/basic14.css \
    test/css/basic3.css \
    test/css/basic6.css \
    test/css/chained1.css \
    test/css/basic7.css \
    test/xml/encoded-attrs/test1.xml \
    test/xml/default-ns/check.txt \
    test/xml/default-ns/input.xml \
    test/xml/encoded-char/check.txt \
    test/xml/encoded-char/input.xml \
    test/xml/bom/check.txt \
    test/xml/bom/input.xml \
    test/xml/cdata-1/check.txt \
    test/xml/cdata-1/input.xml \
    test/xml/doctype/html.xml \
    test/xml/simple/check.txt \
    test/xml/simple/input.xml \
    test/xml/custom-decl-1/check.txt \
    test/xml/custom-decl-1/input.xml \
    test/xml/parse-only/rss/input.xml \
    test/xml/ns-alias-1/check.txt \
    test/xml/ns-alias-1/input.xml \
    test/ods/column-width-row-height/input.ods \
    test/ods/japanese.ods \
    test/ods/formula-1/check.txt \
    test/ods/formula-1/input.ods \
    test/ods/borders/grid-box.ods \
    test/ods/borders/single-cells.ods \
    test/ods/raw-values-1/check.txt \
    test/ods/raw-values-1/input.ods \
    test/ods/date-cell/input.ods \
    test/ods/formatted-text/bold-and-italic.ods \
    test/ods/test.ods

vsprojects_data = \
    vsprojects/orcus-csv/orcus-csv.vcproj \
    vsprojects/orcus-csv/AdditionalLibs.vsprops \
    vsprojects/liborcus-parser.vsprops \
    vsprojects/liborcus-parser-static/liborcus-parser-static.vcproj \
    vsprojects/MddsAndIxion.vsprops \
    vsprojects/liborcus-parser/liborcus-parser.vcproj \
    vsprojects/liborcus-spreadsheet-model/liborcus-spreadsheet-model.vcproj \
    vsprojects/orcus-xlsx/orcus-xlsx.vcproj \
    vsprojects/orcus-xml/orcus-xml.vcproj \
    vsprojects/orcus.sln \
    vsprojects/liborcus/liborcus.vcproj \
    vsprojects/liborcus/zLib.vsprops \
    vsprojects/liborcus/DefaultConfig.vsprops \
    vsprojects/liborcus-static/liborcus-static.vcproj \
    vsprojects/orcus-ods/orcus-ods.vcproj \
    vsprojects/orcus-gnumeric/orcus-gnumeric.vcproj

EXTRA_DIST = \
	liborcus.pc.in \
	autogen.sh \
	$(bin_data) \
	$(doc_data) \
	$(misc_data) \
	$(slickedit_data) \
	$(test_data) \
	$(vsprojects_data)


distclean-local:
	rm -rf *.pc

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog


build-doc-doxygen:
	@echo "Building documentation by doxygen..."
	@cd doc && doxygen doxygen.conf

build-doc-sphinx:
	@echo "Building documentation by sphinx..."
	@sphinx-build -b html ./doc/ ./doc/_build

build-doc: build-doc-doxygen build-doc-sphinx



