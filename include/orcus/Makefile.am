SUBDIRS = mso spreadsheet

liborcusdir = $(includedir)/liborcus-@ORCUS_API_VERSION@/orcus
liborcus_HEADERS = \
	base64.hpp \
	cell_buffer.hpp	\
	css_parser.hpp \
	csv_parser.hpp \
	csv_parser_base.hpp \
	dom_tree.hpp \
	env.hpp \
	exception.hpp \
	format_detection.hpp \
	global.hpp \
	interface.hpp \
	measurement.hpp \
	orcus_csv.hpp \
	orcus_xml.hpp \
	pstring.hpp \
	sax_parser.hpp \
	sax_parser_base.hpp \
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

if WITH_ODS_FILTER

liborcus_HEADERS += \
	orcus_ods.hpp

endif # WITH_ODS_FILTER

if WITH_XLSX_FILTER

liborcus_HEADERS += \
	orcus_xlsx.hpp

endif # WITH_XLSX_FILTER

if WITH_XLS_XML_FILTER

liborcus_HEADERS += \
	orcus_xls_xml.hpp

endif # WITH_XLS_XML_FILTER

if WITH_GNUMERIC_FILTER

liborcus_HEADERS += \
	orcus_gnumeric.hpp

endif # WITH_GNUMERIC_FILTER
