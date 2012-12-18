#!/bin/sh

./bin/gen-odf-tokens.py \
    misc/OpenDocument-v1.2-os-schema.rng \
    src/liborcus/odf_token_constants.inl \
    src/liborcus/odf_tokens.inl \
    src/liborcus/odf_namespace_types

./bin/gen-ooxml-tokens.py -t ooxml \
    misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip \
    src/liborcus/ooxml_token_constants.inl \
    src/liborcus/ooxml_tokens.inl

./bin/gen-ooxml-tokens.py -t opc \
    misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip \
    src/liborcus/opc_token_constants.inl \
    src/liborcus/opc_tokens.inl

./bin/gen-gnumeric-tokens.py \
    misc/gnumeric.xsd \
    src/liborcus/gnumeric_token_constants.inl \
    src/liborcus/gnumeric_tokens.inl
