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

import token_util, argparse, sys


desc = "Generate C++ source files from a list of tokens."

def main ():
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('tokenlist', nargs=1, help='plain-text file that contains a list of tokens.')
    parser.add_argument('output1', nargs=1, help="output file that will contain XML token values.")
    parser.add_argument('output2', nargs=1, help="output file that will contain XML token names.")
    args = parser.parse_args(sys.argv[1:])

    file = open(args.tokenlist[0], 'r')
    tokens = {}
    for line in file.readlines():
        token = line.strip()
        tokens[token] = True

    tokens = tokens.keys()
    tokens.sort()
    token_util.gen_token_constants(args.output1[0], tokens)
    token_util.gen_token_names(args.output2[0], tokens)

    file.close()


if __name__ == '__main__':
    main()
