
INCDIR = $(top_srcdir)/include
SRCDIR = $(top_srcdir)/src/liborcus
BINDIR = $(top_srcdir)/bin

ODF_SCHEMAPATH = $(top_builddir)/misc/OpenDocument-schema-v1.2-cd04.rng
OOXML_SCHEMAPATH = $(top_builddir)/misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip
OPC_SCHEMAPATH = $(top_builddir)/misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip

all: gen

gen:
	$(BINDIR)/gen-odf-tokens.py $(ODF_SCHEMAPATH) $(INCDIR)/orcus/odf/odf_token_constants.inl $(SRCDIR)/odf_tokens.inl $(SRCDIR)/odf_tokens.txt
	$(BINDIR)/gen-ooxml-tokens.py -t ooxml $(OOXML_SCHEMAPATH) \
		$(INCDIR)/orcus/ooxml/ooxml_token_constants.inl $(SRCDIR)/ooxml_tokens.inl $(SRCDIR)/ooxml_tokens.txt
	$(BINDIR)/gen-ooxml-tokens.py -t opc $(OPC_SCHEMAPATH) \
		$(INCDIR)/orcus/ooxml/opc_token_constants.inl $(SRCDIR)/ooxml_opc_tokens.inl $(SRCDIR)/ooxml_opc_tokens.txt
	touch $@

check: all

clean-generic:
	rm -f gen
	rm -f $(SRCDIR)/odf_tokens.inl $(SRCDIR)/odf_tokens.txt
	rm -f $(SRCDIR)/ooxml_tokens.inl $(SRCDIR)/ooxml_tokens.txt
	rm -f $(SRCDIR)/ooxml_opc_tokens.inl $(SRCDIR)/ooxml_opc_tokens.txt
