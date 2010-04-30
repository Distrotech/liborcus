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

OBJDIR=./obj
SRCDIR=./src
INCDIR=./inc
BINDIR=./bin
ROOTDIR=.

ODF_SCHEMA=OpenDocument-schema-v1.2-cd04.rng
ODF_SCHEMAPATH=$(ROOTDIR)/misc/$(ODF_SCHEMA)

CPPFLAGS=-I$(INCDIR) -Os -g -Wall `pkg-config --cflags libgsf-1` -std=c++0x
LDFLAGS=`pkg-config --libs libgsf-1`

HEADERS= \
	$(INCDIR)/global.hpp \
	$(INCDIR)/odf_tokens.hpp \
	$(INCDIR)/odf_token_constants.hpp \
	$(INCDIR)/xmlhandler.hpp \
	$(INCDIR)/odshandler.hpp \
	$(INCDIR)/xmlcontext.hpp \
	$(INCDIR)/odscontext.hpp \
	$(INCDIR)/paracontext.hpp \
	$(INCDIR)/xmlparser.hpp \
	$(INCDIR)/model/odstable.hpp \
	$(INCDIR)/model/global.hpp

OBJFILES= \
	$(OBJDIR)/orcus_ods.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/odf_tokens.o \
	$(OBJDIR)/xmlhandler.o \
	$(OBJDIR)/odshandler.o \
	$(OBJDIR)/xmlcontext.o \
	$(OBJDIR)/odscontext.o \
	$(OBJDIR)/paracontext.o \
	$(OBJDIR)/xmlparser.o \
	$(OBJDIR)/odstable.o

DEPENDS= \
	$(OBJDIR)/gen_odf_tokens \
	$(HEADERS)


all: orcus-ods

pre:
	mkdir $(OBJDIR) 2>/dev/null || /bin/true

$(OBJDIR)/orcus_ods.o: $(SRCDIR)/orcus_ods.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/orcus_ods.cpp

$(OBJDIR)/global.o: $(SRCDIR)/global.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/global.cpp

$(OBJDIR)/xmlparser.o: $(SRCDIR)/xmlparser.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlparser.cpp

$(OBJDIR)/odf_tokens.o: $(SRCDIR)/odf_tokens.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odf_tokens.cpp

$(OBJDIR)/xmlhandler.o: $(SRCDIR)/xmlhandler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlhandler.cpp

$(OBJDIR)/odshandler.o: $(SRCDIR)/odshandler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odshandler.cpp

$(OBJDIR)/xmlcontext.o: $(SRCDIR)/xmlcontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlcontext.cpp

$(OBJDIR)/paracontext.o: $(SRCDIR)/paracontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/paracontext.cpp

$(OBJDIR)/odscontext.o: $(SRCDIR)/odscontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odscontext.cpp

# model directory

$(OBJDIR)/odstable.o: $(SRCDIR)/model/odstable.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/model/odstable.cpp

orcus-ods: pre $(OBJFILES)
	$(CXX) $(LDFLAGS) $(OBJFILES) -o $@

$(OBJDIR)/gen_odf_tokens:
	$(BINDIR)/gen-odf-tokens.py $(ODF_SCHEMAPATH) $(INCDIR)/odf_token_constants.inl $(SRCDIR)/odf_tokens.inl
	touch $@

test.ods: orcus-ods
	./orcus-ods ./test/test.ods $(OBJDIR)/test.ods.html

test.ou: orcus-ods
	./orcus-ods ./test/george-ou-perf.ods $(OBJDIR)/george-ou-perf.ods.html

clean:
	rm -rf $(OBJDIR) 2> /dev/null || /bin/true
	rm $(EXEC) 2> /dev/null || /bin/true

