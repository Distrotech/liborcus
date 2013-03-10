SUBDIRS = mso spreadsheet

liborcusdir = $(includedir)/liborcus-@ORCUS_API_VERSION@/orcus
liborcus_HEADERS = \
	cell_buffer.hpp	\
	css_parser.hpp \
	csv_parser.hpp \
	dom_tree.hpp \
	env.hpp \
	exception.hpp \
	global.hpp \
	orcus_csv.hpp \
	orcus_xml.hpp \
	pstring.hpp \
	sax_parser.hpp \
	sax_parser_global.hpp \
	sax_ns_parser.hpp \
	sax_token_parser.hpp \
	stream.hpp \
	string_pool.hpp \
	tokens.hpp \
	types.hpp \
	xml_namespace.hpp \
	xml_structure_tree.hpp \
	zip_archive.hpp \
	zip_archive_stream.hpp

if WITH_LIBZIP

if WITH_ODS_FILTER

liborcus_HEADERS += \
	orcus_ods.hpp

endif # WITH_ODS_FILTER

if WITH_XLSX_FILTER

liborcus_HEADERS += \
	orcus_xlsx.hpp

endif # WITH_XLSX_FILTER

endif # WITH_LIBZIP

if WITH_GNUMERIC_FILTER

liborcus_HEADERS += \
	orcus_gnumeric.hpp

endif # WITH_GNUMERIC_FILTER
