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

EXECS=orcus-ods orcus-xlsx

OBJDIR=./obj
SRCDIR=./src
INCDIR=./inc
BINDIR=./bin
ROOTDIR=.

ODF_SCHEMA=OpenDocument-schema-v1.2-cd04.rng
ODF_SCHEMAPATH=$(ROOTDIR)/misc/$(ODF_SCHEMA)

OOXML_SCHEMAPATH=$(ROOTDIR)/misc/ooxml-ecma-376/OfficeOpenXML-XMLSchema.zip

CPPFLAGS=-I$(INCDIR) -Os -g -Wall `pkg-config --cflags libgsf-1` -std=c++0x
LDFLAGS=`pkg-config --libs libgsf-1`

HEADERS= \
	$(INCDIR)/global.hpp \
	$(INCDIR)/xmlhandler.hpp \
	$(INCDIR)/xmlcontext.hpp \
	$(INCDIR)/xmlparser.hpp \
	$(INCDIR)/odf/odf_tokens.hpp \
	$(INCDIR)/odf/odf_token_constants.hpp \
	$(INCDIR)/odf/odshandler.hpp \
	$(INCDIR)/odf/odscontext.hpp \
	$(INCDIR)/odf/paracontext.hpp \
	$(INCDIR)/model/odstable.hpp \
	$(INCDIR)/model/global.hpp

OBJFILES= \
	$(OBJDIR)/orcus_ods.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/xmlhandler.o \
	$(OBJDIR)/xmlcontext.o \
	$(OBJDIR)/xmlparser.o \
	$(OBJDIR)/odf/odf_tokens.o \
	$(OBJDIR)/odf/odshandler.o \
	$(OBJDIR)/odf/odscontext.o \
	$(OBJDIR)/odf/paracontext.o \
	$(OBJDIR)/model/odstable.o

DEPENDS= \
	$(OBJDIR)/gen_odf_tokens \
	$(OBJDIR)/gen_ooxml_tokens \
	$(HEADERS)

XLSX_OBJFILES = \
	$(OBJDIR)/orcus_xlsx.o \
	$(OBJDIR)/global.o


all: $(EXECS)

$(OBJDIR)/pre:
	mkdir $(OBJDIR)       2>/dev/null || /bin/true
	mkdir $(OBJDIR)/model 2>/dev/null || /bin/true
	mkdir $(OBJDIR)/odf   2>/dev/null || /bin/true
	touch $@

$(OBJDIR)/orcus_ods.o: $(SRCDIR)/orcus_ods.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/orcus_ods.cpp

$(OBJDIR)/orcus_xlsx.o: $(SRCDIR)/orcus_xlsx.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/orcus_xlsx.cpp

$(OBJDIR)/global.o: $(SRCDIR)/global.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/global.cpp

$(OBJDIR)/xmlcontext.o: $(SRCDIR)/xmlcontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlcontext.cpp

$(OBJDIR)/xmlhandler.o: $(SRCDIR)/xmlhandler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlhandler.cpp

$(OBJDIR)/xmlparser.o: $(SRCDIR)/xmlparser.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xmlparser.cpp

# ODF parser

$(OBJDIR)/odf/odf_tokens.o: $(SRCDIR)/odf/odf_tokens.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odf/odf_tokens.cpp

$(OBJDIR)/odf/odshandler.o: $(SRCDIR)/odf/odshandler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odf/odshandler.cpp

$(OBJDIR)/odf/paracontext.o: $(SRCDIR)/odf/paracontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odf/paracontext.cpp

$(OBJDIR)/odf/odscontext.o: $(SRCDIR)/odf/odscontext.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/odf/odscontext.cpp

# OOXML parser

# model directory

$(OBJDIR)/model/odstable.o: $(SRCDIR)/model/odstable.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/model/odstable.cpp

orcus-ods: $(OBJDIR)/pre $(OBJFILES)
	$(CXX) $(LDFLAGS) $(OBJFILES) -o $@

orcus-xlsx: $(OBJDIR)/pre $(XLSX_OBJFILES)
	$(CXX) $(LDFLAGS) $(XLSX_OBJFILES) -o $@

# token generation

$(OBJDIR)/gen_odf_tokens: $(OBJDIR)/pre
	$(BINDIR)/gen-odf-tokens.py $(ODF_SCHEMAPATH) $(INCDIR)/odf/odf_token_constants.inl $(SRCDIR)/odf/odf_tokens.inl
	touch $@

$(OBJDIR)/gen_ooxml_tokens: $(OBJDIR)/pre
	$(BINDIR)/gen-ooxml-tokens.py $(OOXML_SCHEMAPATH) $(INCDIR)/ooxml/ooxml_token_constants.inl $(SRCDIR)/ooxml/ooxml_tokens.inl
	touch $@

test.ods: orcus-ods
	./orcus-ods ./test/test.ods $(OBJDIR)/test.ods.html

test.ou: orcus-ods
	./orcus-ods ./test/george-ou-perf.ods $(OBJDIR)/george-ou-perf.ods.html

test.xlsx: orcus-xlsx
	./orcus-xlsx ./test/test.xlsx $(OBJDIR)/test.xlsx.html

clean:
	rm -rf $(OBJDIR) 2> /dev/null || /bin/true
	rm $(EXECS) 2> /dev/null || /bin/true

