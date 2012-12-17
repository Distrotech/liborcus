namespace orcus {

const xmlns_id_t NS_odf_anim = "urn:oasis:names:tc:opendocument:xmlns:animation:1.0";
const xmlns_id_t NS_odf_chart = "urn:oasis:names:tc:opendocument:xmlns:chart:1.0";
const xmlns_id_t NS_odf_config = "urn:oasis:names:tc:opendocument:xmlns:config:1.0";
const xmlns_id_t NS_odf_db = "urn:oasis:names:tc:opendocument:xmlns:database:1.0";
const xmlns_id_t NS_odf_dc = "http://purl.org/dc/elements/1.1/";
const xmlns_id_t NS_odf_dr3d = "urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0";
const xmlns_id_t NS_odf_draw = "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0";
const xmlns_id_t NS_odf_fo = "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0";
const xmlns_id_t NS_odf_form = "urn:oasis:names:tc:opendocument:xmlns:form:1.0";
const xmlns_id_t NS_odf_grddl = "http://www.w3.org/2003/g/data-view#";
const xmlns_id_t NS_odf_math = "http://www.w3.org/1998/Math/MathML";
const xmlns_id_t NS_odf_meta = "urn:oasis:names:tc:opendocument:xmlns:meta:1.0";
const xmlns_id_t NS_odf_number = "urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0";
const xmlns_id_t NS_odf_office = "urn:oasis:names:tc:opendocument:xmlns:office:1.0";
const xmlns_id_t NS_odf_presentation = "urn:oasis:names:tc:opendocument:xmlns:presentation:1.0";
const xmlns_id_t NS_odf_script = "urn:oasis:names:tc:opendocument:xmlns:script:1.0";
const xmlns_id_t NS_odf_smil = "urn:oasis:names:tc:opendocument:xmlns:smil-compatible:1.0";
const xmlns_id_t NS_odf_style = "urn:oasis:names:tc:opendocument:xmlns:style:1.0";
const xmlns_id_t NS_odf_svg = "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0";
const xmlns_id_t NS_odf_table = "urn:oasis:names:tc:opendocument:xmlns:table:1.0";
const xmlns_id_t NS_odf_text = "urn:oasis:names:tc:opendocument:xmlns:text:1.0";
const xmlns_id_t NS_odf_xforms = "http://www.w3.org/2002/xforms";
const xmlns_id_t NS_odf_xhtml = "http://www.w3.org/1999/xhtml";
const xmlns_id_t NS_odf_xlink = "http://www.w3.org/1999/xlink";

namespace {

const xmlns_id_t odf_ns[] = {
    NS_odf_anim,
    NS_odf_chart,
    NS_odf_config,
    NS_odf_db,
    NS_odf_dc,
    NS_odf_dr3d,
    NS_odf_draw,
    NS_odf_fo,
    NS_odf_form,
    NS_odf_grddl,
    NS_odf_math,
    NS_odf_meta,
    NS_odf_number,
    NS_odf_office,
    NS_odf_presentation,
    NS_odf_script,
    NS_odf_smil,
    NS_odf_style,
    NS_odf_svg,
    NS_odf_table,
    NS_odf_text,
    NS_odf_xforms,
    NS_odf_xhtml,
    NS_odf_xlink,
    NULL
};

} // anonymous

const xmlns_id_t* NS_odf_all = odf_ns;

}

