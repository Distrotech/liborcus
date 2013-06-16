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

import sys

def normalize_name (old):
    new = ''
    for c in old:
        if c in '.-': # '.' nad '-' are not allowed in C++ symbols.
            c = '_'
        new += c
    return new

unknown_token_name = "??"

def gen_token_list (filepath, tokens, ns_tokens):
    dic = {}
    for t in tokens:
        dic[t] = True
    for t in ns_tokens:
        dic[t] = True

    keys = dic.keys()
    keys.sort()
    file = open(filepath, 'w')
    for key in keys:
        file.write(key + "\n")
    file.close()


def die (msg):
    sys.stderr.write(msg + "\n")
    sys.exit(1)


def get_auto_gen_warning ():
    return "// This file has been auto-generated.  Do not hand-edit this.\n\n"


def gen_token_constants (filepath, tokens):

    outfile = open(filepath, 'w')
    outfile.write(get_auto_gen_warning())

    token_id = 1
    token_size = len(tokens)
    for i in xrange(0, token_size):
        token = normalize_name(tokens[i])
        outfile.write("const xml_token_t XML_%s = %d;\n"%(token, token_id))
        token_id += 1
    outfile.write("\n")
    outfile.close()


def gen_token_names (filepath, tokens):

    outfile = open(filepath, 'w')
    outfile.write(get_auto_gen_warning())

    outfile.write("const char* token_names[] = {\n")
    outfile.write("    \"%s\", // 0\n"%unknown_token_name)
    token_id = 1
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
