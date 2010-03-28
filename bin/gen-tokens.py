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

import xml.parsers.expat, sys

class xml_parser:

    def __init__ (self, strm):
        self.__strm = strm
        self.__elem = None
        self.tokens = {}
        self.ns_tokens = {}  # namespace tokens

    def start_element(self, name, attrs):
        self.__elem = name
        if name in ['element', 'attribute'] and attrs.has_key('name'):
            tokens = attrs['name'].split(':')
            n = len(tokens)
            if n == 1:
                # namespace-less token
                self.tokens[tokens[0]] = True
            elif n == 2:
                # namespaced token
                self.ns_tokens[tokens[0]] = True
                self.tokens[tokens[1]] = True
            else:
                sys.stderr.write("unrecognized token type: "+attrs['name'])
                sys.exit(1)


            for token in tokens:
                self.tokens[token] = True

    def end_element(self, name):
        pass

    def character(self, data):
        if self.__elem == 'value':
            s = data.strip()
            if len(s) > 0:
                self.tokens[s] = True

    def parse (self):
        p = xml.parsers.expat.ParserCreate()
        p.StartElementHandler = self.start_element
        p.EndElementHandler = self.end_element
        p.CharacterDataHandler = self.character
        p.Parse(self.__strm, 1)

def normalize_name (old):
    new = ''
    for c in old:
        if c in '.-': # '.' nad '-' are not allowed in C++ symbols.
            c = '_'
        new += c
    return new

def get_auto_gen_warning ():
    return "// This file has been auto-generated.  Do not hand-edit this.\n\n"

def gen_token_constants (filepath, tokens, ns_tokens):

    outfile = open(filepath, 'w')
    outfile.write(get_auto_gen_warning())
    outfile.write("namespace orcus {\n\n")

    outfile.write("enum xml_token_t {\n")
    token_id = 0
    token_size = len(tokens)
    for i in xrange(0, token_size):
        token = normalize_name(tokens[i])
        outfile.write("    XML_%s = %d,\n"%(token, token_id))
        token_id += 1
    outfile.write("\n    XML_UNKNOWN_TOKEN = %d // special token to handle unrecognized token names.\n"%token_id)
    outfile.write("};\n\n")

    outfile.write("enum xmlns_token_t {\n")
    token_id = 0
    token_size = len(ns_tokens)
    for i in xrange(0, token_size):
        token = normalize_name(ns_tokens[i])
        outfile.write("    XMLNS_%s = %d,\n"%(token, token_id))
        token_id += 1
    outfile.write("\n    XMLNS_UNKNOWN_TOKEN = %d // special token to handle unrecognized token names.\n"%token_id)
    outfile.write("};\n\n")

    outfile.write("}\n")
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
    outfile.close()

def main (args):

    file = open(sys.argv[1], 'r')
    chars = file.read()
    file.close()
    
    parser = xml_parser(chars)
    parser.parse()
    tokens = parser.tokens.keys()
    tokens.sort()
    ns_tokens = parser.ns_tokens.keys()
    ns_tokens.sort()

    gen_token_constants(sys.argv[2], tokens, ns_tokens)
    gen_token_names(sys.argv[3], tokens)

if __name__ == '__main__':
    main(sys.argv)
