#************************************************************************
#
# Copyright (c) 2010 Kohei Yoshida
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
#***********************************************************************

EXEC=orcus-test
OBJDIR=./obj
SRCDIR=./src
INCDIR=./inc

CPPFLAGS=-I$(INCDIR) -g -Wall `pkg-config --cflags libgsf-1`
LDFLAGS=`pkg-config --libs libgsf-1`

HEADERS= \
	$(INCDIR)/global.hpp

OBJFILES= \
	$(OBJDIR)/main.o

all: $(EXEC)

pre:
	mkdir $(OBJDIR) 2>/dev/null || /bin/true

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/main.cpp

$(EXEC): pre $(OBJFILES)
	$(CXX) $(LDFLAGS) $(OBJFILES) -o $(EXEC)

test: $(EXEC)
	./$(EXEC) ./test/test.ods

clean:
	rm -rf $(OBJDIR)
	rm $(EXEC)

