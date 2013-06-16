#!/usr/bin/env python
#************************************************************************
#
#  Copyright (c) 2010 Kohei Yoshida
#  
#  Permission is hereby granted, free of charge, to any person
#  obtaining a copy of this software and associated documentation
#  files (the "Software"), to deal in the Software without
#  restriction, including without limitation the rights to use,
#  copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following
#  conditions:
#  
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#  
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#  OTHER DEALINGS IN THE SOFTWARE.
#
#***********************************************************************

import xml.parsers.expat, zipfile, optparse, sys
import token_util

class xml_parser:

    def __init__ (self, strm):
        self.__strm = strm
        self.__elem = None
        self.tokens = []

    def start_element(self, name, attrs):
        self.__elem = name
        if name in ['xs:element', 'xs:attribute', 'xsd:element', 'xsd:attribute'] and attrs.has_key('name'):
            token = attrs['name']
            if len(token) > 0:
                self.tokens.append(token)

    def end_element(self, name):
        pass

    def character(self, data):
        pass

    def parse (self):
        p = xml.parsers.expat.ParserCreate()
        p.StartElementHandler = self.start_element
        p.EndElementHandler = self.end_element
        p.CharacterDataHandler = self.character
        p.Parse(self.__strm, 1)


def get_all_tokens_from_zip (fpath):
    zip = zipfile.ZipFile(fpath, 'r')
    tokens = {}
    for item in zip.namelist():
        fd = zip.open(item, 'r')
        parser = xml_parser(fd.read())
        fd.close()
        parser.parse()
        for token in parser.tokens:
            tokens[token] = True
    zip.close()

    keys = tokens.keys()
    keys.sort()
    return keys


def main ():
    parser = optparse.OptionParser()
    parser.add_option("-t", "--schema-type", dest="schema_type", default="ooxml", metavar="TYPE",
        help="Specify the schema type.  Possible values are: 'ooxml', or 'opc'.  The default value is 'ooxml'.")
    options, args = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(1)

    schema_type = options.schema_type
    if not schema_type in ['opc', 'ooxml']:
        token_util.die("Unsupported schema type: %s"%schema_type)

    tokens = get_all_tokens_from_zip(args[0])
    token_util.gen_token_constants(args[1], tokens)
    token_util.gen_token_names(args[2], tokens)

if __name__ == '__main__':
    main()
