
if WITH_LIBZIP

liborcusdir = $(includedir)/liborcus-@ORCUS_API_VERSION@/orcus/ooxml

liborcus_HEADERS = \
	content_types.hpp \
	global.hpp \
	namespace_types.hpp \
	ooxml_token_constants.hpp \
	ooxml_token_constants.inl \
	ooxml_tokens.hpp \
	ooxml_types.hpp \
	opc_context.hpp \
	opc_token_constants.hpp \
	opc_token_constants.inl \
	schemas.hpp

if WITH_XLSX_FILTER

liborcus_HEADERS += \
	xlsx_context.hpp \
	xlsx_handler.hpp \
	xlsx_sheet_context.hpp \
	xlsx_types.hpp \
	xlsx_workbook_context.hpp

endif # WITH_XLSX_FILTER

endif # WITH_LIBZIP
