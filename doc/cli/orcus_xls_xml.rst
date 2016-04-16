
orcus-xls-xml
=============

Usage
-----

**orcus-xls-xml [options] FILE**

The FILE must specify a path to an existing file.

Allowed options
---------------

**-h [ --help ]**
   Print this help.

**-d [ --debug ]**
   Turn on a debug mode to generate run-time debug output.

**--dump-check**
   Dump the the content to stdout in a special format used for content
   verification in unit tests.

**-o [ --output ] arg**
   Output directory path, or output file when --dump-check option is used.

**-f [ --output-format ] arg**
   Specify the format of output file.  Supported format types are:

   - flat text format (flat)
   - HTML format (html)
   - no output (none)

