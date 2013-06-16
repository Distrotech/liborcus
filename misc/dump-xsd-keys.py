#!/usr/bin/env python
########################################################################
#
#  Copyright (c) 2013 Kohei Yoshida
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
########################################################################

import xml.parsers.expat, argparse, sys
import token_util

class xml_parser:

    def __init__ (self, strm):
        self.__strm = strm
        self.__elem = None
        self.tokens = []
        self.ns_tokens = []

    def start_element(self, name, attrs):
        self.__elem = name
        if name in ['xs:element', 'xs:attribute', 'xsd:element', 'xsd:attribute'] and attrs.has_key('name'):
            token = attrs['name']
            if len(token) > 0:
                self.tokens.append(token)

        if name.endswith(':schema'):
            # Check for namespace entries.
            attr_names = attrs.keys()
            for attr_name in attr_names:
                if attr_name == 'xmlns':
                    self.ns_tokens.append(['', attrs[attr_name]])
                elif attr_name.startswith('xmlns:'):
                    vals = attr_name.split(':')
                    self.ns_tokens.append([vals[1], attrs[attr_name]])

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


def parse_files(filenames):

    tokens = {}
    for filename in filenames:
        file = open(filename, 'r')
        chars = file.read()
        file.close()

        parser = xml_parser(chars)
        parser.parse()
        for token in parser.tokens:
            tokens[token] = True

    keys = tokens.keys()
    keys.sort()

    return keys


def parse_files_ns(filenames):

    tokens = {}
    for filename in filenames:
        file = open(filename, 'r')
        chars = file.read()
        file.close()

        parser = xml_parser(chars)
        parser.parse()
        for ns_token in parser.ns_tokens:
            alias, value = ns_token # each namespace token consists of an alias and a value.
            if not tokens.has_key(value):
                tokens[value] = []
            if len(alias) > 0 and not alias in tokens[value]:
                tokens[value].append(alias)

    keys = tokens.keys()
    keys.sort()
    ret_val = []
    for key in keys:
        aliases = tokens[key]
        t = key + " ("
        first = True
        for alias in aliases:
            if first:
                first = False
            else:
                t += ", "
            t += alias
        t += ")"
        ret_val.append(t)

    return ret_val


desc = "Given an arbitrary XML Schema file (.xsd), dump all its keys specified in the schema to stdout."

def main ():
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('file', nargs='*', help='XML Shema file (.xsd)')
    parser.add_argument('--ns', dest='ns_mode', action='store_true', default=False)
    args = parser.parse_args(sys.argv[1:])
    ns_mode = args.ns_mode

    if ns_mode:
        keys = parse_files_ns(args.file)
    else:
        keys = parse_files(args.file)

    for key in keys:
        print(key)

if __name__ == '__main__':
    main()

