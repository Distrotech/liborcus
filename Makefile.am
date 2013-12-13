SUBDIRS = src include
ACLOCAL_AMFLAGS = -I m4

pcfiles = liborcus-@ORCUS_API_VERSION@.pc
if BUILD_SPREADSHEET_MODEL
pcfiles += liborcus-spreadsheet-model-@ORCUS_API_VERSION@.pc
endif

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

doc_data = \
    doc/rfc4180-csv.txt \
    doc/cell-styles.odg \
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
    test/gnumeric/test.gnumeric \
    test/gnumeric/test.xml \
    test/xml-structure/nested-repeat-1/check.txt \
    test/xml-structure/nested-repeat-1/input.xml \
    test/xml-structure/namespace-default/check.txt \
    test/xml-structure/namespace-default/input.xml \
    test/xml-structure/basic-1/check.txt \
    test/xml-structure/basic-1/input.xml \
    test/xml-structure/attribute-1/check.txt \
    test/xml-structure/attribute-1/input.xml \
    test/xml-structure/basic-3/check.txt \
    test/xml-structure/basic-3/input.xml \
    test/xml-structure/basic-2/check.txt \
    test/xml-structure/basic-2/input.xml \
    test/css/test-complex.css \
    test/css/test.css \
    test/xml/bom/check.txt \
    test/xml/bom/input.xml \
    test/xml/default-ns/check.txt \
    test/xml/default-ns/input.xml \
    test/xml/ns-alias-1/check.txt \
    test/xml/ns-alias-1/input.xml \
    test/xml/parse-only/rss/input.xml \
    test/xml/custom-decl-1/check.txt \
    test/xml/custom-decl-1/input.xml \
    test/xml/encoded-attrs/test1.xml \
    test/xml/cdata-1/check.txt \
    test/xml/cdata-1/input.xml \
    test/xml/simple/check.txt \
    test/xml/simple/input.xml \
    test/xml/doctype/html.xml \
    test/xml/encoded-char/check.txt \
    test/xml/encoded-char/input.xml \
    test/xls-xml/basic/check.txt \
    test/xls-xml/basic/input.xml \
    test/csv/simple-numbers/input.csv \
    test/csv/simple-numbers/check.txt \
    test/csv/quoted-with-delim/input.csv \
    test/csv/quoted-with-delim/check.txt \
    test/csv/normal-quotes/input.csv \
    test/csv/normal-quotes/check.txt \
    test/csv/double-quotes/input.csv \
    test/csv/double-quotes/check.txt \
    test/xlsx/date-cell/input.xlsx \
    test/xlsx/column-width-row-height/input.xlsx \
    test/xlsx/column-width-row-height/xml/[Content_Types].xml \
    test/xlsx/column-width-row-height/xml/xl/styles.xml \
    test/xlsx/column-width-row-height/xml/xl/worksheets/sheet2.xml \
    test/xlsx/column-width-row-height/xml/xl/worksheets/sheet1.xml \
    test/xlsx/column-width-row-height/xml/xl/worksheets/sheet3.xml \
    test/xlsx/column-width-row-height/xml/xl/sharedStrings.xml \
    test/xlsx/column-width-row-height/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/column-width-row-height/xml/xl/theme/theme1.xml \
    test/xlsx/column-width-row-height/xml/xl/workbook.xml \
    test/xlsx/column-width-row-height/xml/docProps/app.xml \
    test/xlsx/column-width-row-height/xml/docProps/core.xml \
    test/xlsx/column-width-row-height/xml/_rels/.rels \
    test/xlsx/formula-simple.xlsx \
    test/xlsx/formula-shared.xlsx \
    test/xlsx/data-table/one-variable.xlsx \
    test/xlsx/data-table/xml/[Content_Types].xml \
    test/xlsx/data-table/xml/xl/styles.xml \
    test/xlsx/data-table/xml/xl/worksheets/sheet1.xml \
    test/xlsx/data-table/xml/xl/worksheets/_rels/sheet1.xml.rels \
    test/xlsx/data-table/xml/xl/sharedStrings.xml \
    test/xlsx/data-table/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/data-table/xml/xl/theme/theme1.xml \
    test/xlsx/data-table/xml/xl/workbook.xml \
    test/xlsx/data-table/xml/xl/calcChain.xml \
    test/xlsx/data-table/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/data-table/xml/docProps/app.xml \
    test/xlsx/data-table/xml/docProps/core.xml \
    test/xlsx/data-table/xml/_rels/.rels \
    test/xlsx/data-table/multi-table.xlsx \
    test/xlsx/xml/[Content_Types].xml \
    test/xlsx/xml/xl/styles.xml \
    test/xlsx/xml/xl/worksheets/sheet2.xml \
    test/xlsx/xml/xl/worksheets/sheet1.xml \
    test/xlsx/xml/xl/worksheets/_rels/sheet2.xml.rels \
    test/xlsx/xml/xl/worksheets/sheet3.xml \
    test/xlsx/xml/xl/sharedStrings.xml \
    test/xlsx/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/xml/xl/theme/theme1.xml \
    test/xlsx/xml/xl/workbook.xml \
    test/xlsx/xml/xl/calcChain.xml \
    test/xlsx/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/xml/docProps/app.xml \
    test/xlsx/xml/docProps/core.xml \
    test/xlsx/xml/_rels/.rels \
    test/xlsx/empty-shared-strings/input.xlsx \
    test/xlsx/empty-shared-strings/check.txt \
    test/xlsx/number-format/xml/[Content_Types].xml \
    test/xlsx/number-format/xml/xl/styles.xml \
    test/xlsx/number-format/xml/xl/worksheets/sheet2.xml \
    test/xlsx/number-format/xml/xl/worksheets/sheet1.xml \
    test/xlsx/number-format/xml/xl/worksheets/_rels/sheet1.xml.rels \
    test/xlsx/number-format/xml/xl/worksheets/sheet3.xml \
    test/xlsx/number-format/xml/xl/sharedStrings.xml \
    test/xlsx/number-format/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/number-format/xml/xl/theme/theme1.xml \
    test/xlsx/number-format/xml/xl/workbook.xml \
    test/xlsx/number-format/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/number-format/xml/docProps/app.xml \
    test/xlsx/number-format/xml/docProps/core.xml \
    test/xlsx/number-format/xml/_rels/.rels \
    test/xlsx/number-format/basic.xlsx \
    test/xlsx/table/xml/[Content_Types].xml \
    test/xlsx/table/xml/xl/styles.xml \
    test/xlsx/table/xml/xl/worksheets/sheet1.xml \
    test/xlsx/table/xml/xl/worksheets/_rels/sheet1.xml.rels \
    test/xlsx/table/xml/xl/sharedStrings.xml \
    test/xlsx/table/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/table/xml/xl/theme/theme1.xml \
    test/xlsx/table/xml/xl/workbook.xml \
    test/xlsx/table/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/table/xml/docProps/app.xml \
    test/xlsx/table/xml/docProps/core.xml \
    test/xlsx/table/xml/_rels/.rels \
    test/xlsx/table/autofilter.xlsx \
    test/xlsx/formatted-text/colored-text.xlsx \
    test/xlsx/formatted-text/xml/[Content_Types].xml \
    test/xlsx/formatted-text/xml/xl/styles.xml \
    test/xlsx/formatted-text/xml/xl/worksheets/sheet1.xml \
    test/xlsx/formatted-text/xml/xl/worksheets/_rels/sheet1.xml.rels \
    test/xlsx/formatted-text/xml/xl/sharedStrings.xml \
    test/xlsx/formatted-text/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/formatted-text/xml/xl/theme/theme1.xml \
    test/xlsx/formatted-text/xml/xl/workbook.xml \
    test/xlsx/formatted-text/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/formatted-text/xml/docProps/app.xml \
    test/xlsx/formatted-text/xml/docProps/core.xml \
    test/xlsx/formatted-text/xml/_rels/.rels \
    test/xlsx/formatted-text/bold-and-italic.xlsx \
    test/xlsx/raw-values-1/input.xlsx \
    test/xlsx/raw-values-1/check.txt \
    test/xlsx/background-color/standard.xlsx \
    test/xlsx/background-color/xml/[Content_Types].xml \
    test/xlsx/background-color/xml/xl/styles.xml \
    test/xlsx/background-color/xml/xl/worksheets/sheet1.xml \
    test/xlsx/background-color/xml/xl/worksheets/_rels/sheet1.xml.rels \
    test/xlsx/background-color/xml/xl/sharedStrings.xml \
    test/xlsx/background-color/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/background-color/xml/xl/theme/theme1.xml \
    test/xlsx/background-color/xml/xl/workbook.xml \
    test/xlsx/background-color/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/background-color/xml/docProps/app.xml \
    test/xlsx/background-color/xml/docProps/core.xml \
    test/xlsx/background-color/xml/_rels/.rels \
    test/xlsx/borders/grid-box.xlsx \
    test/xlsx/borders/single-cells.xlsx \
    test/xlsx/borders/xml/[Content_Types].xml \
    test/xlsx/borders/xml/xl/styles.xml \
    test/xlsx/borders/xml/xl/worksheets/sheet2.xml \
    test/xlsx/borders/xml/xl/worksheets/sheet1.xml \
    test/xlsx/borders/xml/xl/worksheets/_rels/sheet1.xml.rels \
    test/xlsx/borders/xml/xl/worksheets/sheet3.xml \
    test/xlsx/borders/xml/xl/sharedStrings.xml \
    test/xlsx/borders/xml/xl/_rels/workbook.xml.rels \
    test/xlsx/borders/xml/xl/theme/theme1.xml \
    test/xlsx/borders/xml/xl/workbook.xml \
    test/xlsx/borders/xml/xl/printerSettings/printerSettings1.bin \
    test/xlsx/borders/xml/docProps/app.xml \
    test/xlsx/borders/xml/docProps/core.xml \
    test/xlsx/borders/xml/_rels/.rels \
    test/xlsx/test.xlsx \
    test/ods/date-cell/input.ods \
    test/ods/column-width-row-height/xml/styles.xml \
    test/ods/column-width-row-height/xml/META-INF/manifest.xml \
    test/ods/column-width-row-height/xml/content.xml \
    test/ods/column-width-row-height/xml/mimetype \
    test/ods/column-width-row-height/xml/meta.xml \
    test/ods/column-width-row-height/xml/settings.xml \
    test/ods/column-width-row-height/xml/Configurations2/accelerator/current.xml \
    test/ods/column-width-row-height/xml/Thumbnails/thumbnail.png \
    test/ods/column-width-row-height/input.ods \
    test/ods/test.ods \
    test/ods/japanese.ods \
    test/ods/formula.ods \
    test/ods/formatted-text/xml/styles.xml \
    test/ods/formatted-text/xml/META-INF/manifest.xml \
    test/ods/formatted-text/xml/content.xml \
    test/ods/formatted-text/xml/mimetype \
    test/ods/formatted-text/xml/meta.xml \
    test/ods/formatted-text/xml/settings.xml \
    test/ods/formatted-text/xml/Configurations2/accelerator/current.xml \
    test/ods/formatted-text/xml/Thumbnails/thumbnail.png \
    test/ods/formatted-text/bold-and-italic.ods \
    test/ods/raw-values-1/check.txt \
    test/ods/raw-values-1/input.ods \
    test/ods/borders/xml/styles.xml \
    test/ods/borders/xml/META-INF/manifest.xml \
    test/ods/borders/xml/content.xml \
    test/ods/borders/xml/mimetype \
    test/ods/borders/xml/meta.xml \
    test/ods/borders/xml/settings.xml \
    test/ods/borders/xml/Configurations2/accelerator/current.xml \
    test/ods/borders/xml/Thumbnails/thumbnail.png \
    test/ods/borders/grid-box.ods \
    test/ods/borders/single-cells.ods \
    test/xml-mapped/content-basic/map.xml \
    test/xml-mapped/content-basic/check.txt \
    test/xml-mapped/content-basic/input.xml \
    test/xml-mapped/attribute-basic/map.xml \
    test/xml-mapped/attribute-basic/check.txt \
    test/xml-mapped/attribute-basic/input.xml \
    test/xml-mapped/attribute-single-element-2/map.xml \
    test/xml-mapped/attribute-single-element-2/check.txt \
    test/xml-mapped/attribute-single-element-2/input.xml \
    test/xml-mapped/attribute-single-element/map.xml \
    test/xml-mapped/attribute-single-element/check.txt \
    test/xml-mapped/attribute-single-element/input.xml \
    test/xml-mapped/attribute-namespace/map.xml \
    test/xml-mapped/attribute-namespace/check.txt \
    test/xml-mapped/attribute-namespace/input.xml \
    test/xml-mapped/content-namespace/map.xml \
    test/xml-mapped/content-namespace/check.txt \
    test/xml-mapped/content-namespace/input.xml \
    test/xml-mapped/attribute-range-self-close/map.xml \
    test/xml-mapped/attribute-range-self-close/check.txt \
    test/xml-mapped/attribute-range-self-close/input.xml \
    test/xml-mapped/content-namespace-2/map.xml \
    test/xml-mapped/content-namespace-2/check.txt \
    test/xml-mapped/content-namespace-2/input.xml

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
