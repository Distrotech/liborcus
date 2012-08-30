#!/bin/sh

./bin/gen-odf-tokens.py \
    misc/OpenDocument-schema-v1.2-cd04.rng \
    include/orcus/odf/odf_token_constants.inl \
    src/liborcus/odf_tokens.inl \
    src/liborcus/odf_tokens.txt

./bin/gen-ooxml-tokens.py -t ooxml \
    misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip \
    include/orcus/ooxml/ooxml_token_constants.inl \
    src/liborcus/ooxml_tokens.inl \
    src/liborcus/ooxml_tokens.txt

./bin/gen-ooxml-tokens.py -t opc \
    misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip \
    include/orcus/ooxml/opc_token_constants.inl \
    src/liborcus/ooxml_opc_tokens.inl \
    src/liborcus/ooxml_opc_tokens.txt
