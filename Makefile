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
BINDIR=./bin
ROOTDIR=.

SCHEMA=OpenDocument-schema-v1.2-cd04.rng
SCHEMAPATH=$(ROOTDIR)/misc/$(SCHEMA)

CPPFLAGS=-I$(INCDIR) -Os -g -Wall `pkg-config --cflags libgsf-1` -std=c++0x
LDFLAGS=`pkg-config --libs libgsf-1`

HEADERS= \
	$(INCDIR)/global.hpp \
	$(INCDIR)/tokens.hpp \
	$(INCDIR)/token_constants.hpp \
	$(INCDIR)/xmlhandler.hpp \
	$(INCDIR)/odshandler.hpp \
	$(INCDIR)/xmlcontext.hpp \
	$(INCDIR)/odscontext.hpp \
	$(INCDIR)/xmlparser.hpp \
	$(INCDIR)/model/odstable.hpp

OBJFILES= \
	$(OBJDIR)/main.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/tokens.o \
	$(OBJDIR)/xmlhandler.o \
	$(OBJDIR)/odshandler.o \
	$(OBJDIR)/xmlcontext.o \
	$(OBJDIR)/odscontext.o \
	$(OBJDIR)/xmlparser.o \
	$(OBJDIR)/odstable.o

DEPENDS= \
	$(OBJDIR)/gen_tokens \
	$(HEADERS)


all: $(EXEC)

pre:
	mkdir $(OBJDIR) 2>/dev/null || /bin/true

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/main.cpp

$(OBJDIR)/global.o: $(SRCDIR)/global.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/global.cpp

$(OBJDIR)/xmlparser.o: $(SRCDIR)/xmlparser.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlparser.cpp

$(OBJDIR)/tokens.o: $(SRCDIR)/tokens.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/tokens.cpp

$(OBJDIR)/xmlhandler.o: $(SRCDIR)/xmlhandler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlhandler.cpp

$(OBJDIR)/odshandler.o: $(SRCDIR)/odshandler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odshandler.cpp

$(OBJDIR)/xmlcontext.o: $(SRCDIR)/xmlcontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlcontext.cpp

$(OBJDIR)/odscontext.o: $(SRCDIR)/odscontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odscontext.cpp

# model directory

$(OBJDIR)/odstable.o: $(SRCDIR)/model/odstable.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/model/odstable.cpp

$(EXEC): pre $(OBJFILES)
	$(CXX) $(LDFLAGS) $(OBJFILES) -o $(EXEC)

$(OBJDIR)/gen_tokens:
	$(BINDIR)/gen-tokens.py $(SCHEMAPATH) $(INCDIR)/token_constants.inl $(SRCDIR)/tokens.inl
	touch $@

test: $(EXEC)
	./$(EXEC) ./test/test.ods $(OBJDIR)/test.ods.html

test.ou: $(EXEC)
	./$(EXEC) ./test/george-ou-perf.ods $(OBJDIR)/george-ou-perf.ods.html

clean:
	rm -rf $(OBJDIR) 2> /dev/null || /bin/true
	rm $(EXEC) 2> /dev/null || /bin/true

