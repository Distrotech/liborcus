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

import xml.parsers.expat, zipfile, sys
import token_util

class xml_parser:

    def __init__ (self, strm):
        self.__strm = strm
        self.__elem = None
        self.tokens = []

    def start_element(self, name, attrs):
        self.__elem = name
        if name in ['xsd:element', 'xsd:attribute'] and attrs.has_key('name'):
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



def get_auto_gen_warning ():
    return "// This file has been auto-generated.  Do not hand-edit this.\n\n"


def gen_ooxml_namespaces ():
    nstokens = ['xmlns', 'spreadsheetML', 'r']
    return nstokens


def gen_token_constants (filepath, tokens):

    outfile = open(filepath, 'w')
    outfile.write(get_auto_gen_warning())

    outfile.write("enum xml_token_t {\n")
    token_id = 0
    token_size = len(tokens)
    for i in xrange(0, token_size):
        token = token_util.normalize_name(tokens[i])
        outfile.write("    XML_%s = %d,\n"%(token, token_id))
        token_id += 1
    outfile.write("\n    XML_UNKNOWN_TOKEN = %d // special token to handle unrecognized token names.\n"%token_id)
    outfile.write("};\n\n")

    ns_tokens = gen_ooxml_namespaces()
    outfile.write("enum xmlns_token_t {\n")
    token_id = 0
    token_size = len(ns_tokens)
    for i in xrange(0, token_size):
        token = token_util.normalize_name(ns_tokens[i])
        outfile.write("    XMLNS_%s = %d,\n"%(token, token_id))
        token_id += 1
    outfile.write("\n    XMLNS_UNKNOWN_TOKEN = %d // special token to handle unrecognized token names.\n"%token_id)
    outfile.write("};\n\n")

    outfile.close()


def gen_token_names (filepath, tokens):

    outfile = open(filepath, 'w')
    outfile.write(get_auto_gen_warning())

    outfile.write("const char* token_names[] = {\n")
    token_id = 0
    token_size = len(tokens)
    for i in xrange(0, token_size):
        token = tokens[i]
        s = ','
        if i == token_size-1:
            s = ' '
        outfile.write("    \"%s\"%s // %d\n"%(token, s, token_id))
        token_id += 1
    outfile.write("};\n\n")
    outfile.write("size_t token_name_count = %d;\n\n"%token_id)

    outfile.write("const char* nstoken_names[] = {\n")
    token_id = 0
    ns_tokens = gen_ooxml_namespaces()
    token_size = len(ns_tokens)
    for i in xrange(0, token_size):
        token = ns_tokens[i]
        s = ','
        if i == token_size-1:
            s = ' '
        outfile.write("    \"%s\"%s // %d\n"%(token, s, token_id))
        token_id += 1
    outfile.write("};\n\n")
    outfile.write("size_t nstoken_name_count = %d;\n\n"%token_id)

    outfile.close()


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


def main (args):
    if len(args) < 4:
        return

    tokens = get_all_tokens_from_zip(args[1])
    gen_token_constants(args[2], tokens)
    gen_token_names(args[3], tokens)

if __name__ == '__main__':
    main(sys.argv)
