
INCDIR = $(top_srcdir)/include
SRCDIR = $(top_srcdir)/src/liborcus
BINDIR = $(top_srcdir)/bin

INCLUDES = -I$(top_srcdir)/include

ODF_SCHEMAPATH = $(top_builddir)/misc/OpenDocument-schema-v1.2-cd04.rng
OOXML_SCHEMAPATH = $(top_builddir)/misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip
OPC_SCHEMAPATH = $(top_builddir)/misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip

pre:
	$(BINDIR)/gen-odf-tokens.py $(ODF_SCHEMAPATH) $(INCDIR)/orcus/odf/odf_token_constants.inl $(SRCDIR)/odf_tokens.inl $(SRCDIR)/odf_tokens.txt
	$(BINDIR)/gen-ooxml-tokens.py -t ooxml $(OOXML_SCHEMAPATH) \
		$(INCDIR)/orcus/ooxml/ooxml_token_constants.inl $(SRCDIR)/ooxml_tokens.inl $(SRCDIR)/ooxml_tokens.txt
	$(BINDIR)/gen-ooxml-tokens.py -t opc $(OPC_SCHEMAPATH) \
		$(INCDIR)/orcus/ooxml/opc_token_constants.inl $(SRCDIR)/ooxml_opc_tokens.inl $(SRCDIR)/ooxml_opc_tokens.txt
	touch $@

odf_tokens.cpp: pre
ooxml_tokens.cpp: pre

lib_LTLIBRARIES = liborcus-@ORCUS_API_VERSION@.la
liborcus_@ORCUS_API_VERSION@_la_SOURCES = \
	global.cpp \
	model_document.cpp \
	model_factory.cpp \
	model_formula_context.cpp \
	model_shared_strings.cpp \
	model_sheet.cpp \
	model_styles.cpp \
	odf_ods_context.cpp \
	odf_ods_handler.cpp \
	odf_para_context.cpp \
	odf_tokens.cpp \
	ooxml_content_types.cpp \
	ooxml_global.cpp \
	ooxml_opc_context.cpp \
	ooxml_schemas.cpp \
	ooxml_tokens.cpp \
	ooxml_xlsx_context.cpp \
	ooxml_xlsx_handler.cpp \
	ooxml_xlsx_sheet_context.cpp \
	ooxml_xlsx_workbook_context.cpp \
	orcus_css.cpp \
	orcus_csv.cpp \
	orcus_ods.cpp \
	orcus_xlsx.cpp \
	pstring.cpp \
	tokens.cpp \
	xml_context.cpp \
	xml_handler.cpp \
	xml_parser.cpp \
	xml_simple_handler.cpp

liborcus_@ORCUS_API_VERSION@_la_CPPFLAGS = `@PKG_CONFIG_CPPFLAGS@`
liborcus_@ORCUS_API_VERSION@_la_LDFLAGS = -no-undefined `@PKG_CONFIG_LDFLAGS@`

liborcus_@ORCUS_API_VERSION@_la_LIBADD =

