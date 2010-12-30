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
OPC_SCHEMAPATH=$(ROOTDIR)/misc/ooxml-ecma-376/OpenPackagingConventions-XMLSchema.zip

CPPFLAGS=-I$(INCDIR) -O2 -g -Wall `pkg-config --cflags libgsf-1` -std=c++0x
LDFLAGS=`pkg-config --libs libgsf-1` -lboost_thread

COMMON_HEADERS= \
	$(INCDIR)/xml_handler.hpp \
	$(INCDIR)/xml_simple_handler.hpp \
	$(INCDIR)/xml_context.hpp \
	$(INCDIR)/xml_parser.hpp \
	$(INCDIR)/global.hpp \
	$(INCDIR)/pstring.hpp \
	$(INCDIR)/tokens.hpp \
	$(INCDIR)/sax.hpp \
	$(INCDIR)/types.hpp

COMMON_OBJFILES= \
	$(OBJDIR)/global.o \
	$(OBJDIR)/xml_handler.o \
	$(OBJDIR)/xml_simple_handler.o \
	$(OBJDIR)/xml_context.o \
	$(OBJDIR)/xml_parser.o \
	$(OBJDIR)/tokens.o \
	$(OBJDIR)/pstring.o

ODF_HEADERS= \
	$(COMMON_HEADERS) \
	$(INCDIR)/types.hpp \
	$(INCDIR)/odf/odf_token_constants.hpp \
	$(INCDIR)/odf/odshandler.hpp \
	$(INCDIR)/odf/odscontext.hpp \
	$(INCDIR)/odf/paracontext.hpp \
	$(INCDIR)/model/sheet.hpp \
	$(INCDIR)/model/global.hpp

ODF_OBJFILES= \
	$(COMMON_OBJFILES) \
	$(OBJDIR)/orcus_ods.o \
	$(OBJDIR)/odf/odf_tokens.o \
	$(OBJDIR)/odf/odshandler.o \
	$(OBJDIR)/odf/odscontext.o \
	$(OBJDIR)/odf/paracontext.o \
	$(OBJDIR)/model/sheet.o

XLSX_HEADERS= \
	$(COMMON_HEADERS) \
	$(INCDIR)/ooxml/ooxml_token_constants.hpp \
	$(INCDIR)/ooxml/opc_context.hpp \
	$(INCDIR)/ooxml/content_types.hpp \
	$(INCDIR)/ooxml/global.hpp \
	$(INCDIR)/ooxml/schemas.hpp \
	$(INCDIR)/ooxml/xlsx_context.hpp \
	$(INCDIR)/ooxml/xlsx_handler.hpp \
	$(INCDIR)/ooxml/ooxml_types.hpp
	
XLSX_OBJFILES = \
	$(COMMON_OBJFILES) \
	$(OBJDIR)/orcus_xlsx.o \
	$(OBJDIR)/ooxml/ooxml_tokens.o \
	$(OBJDIR)/ooxml/opc_context.o \
	$(OBJDIR)/ooxml/xlsx_handler.o \
	$(OBJDIR)/ooxml/xlsx_context.o \
	$(OBJDIR)/ooxml/content_types.o \
	$(OBJDIR)/ooxml/global.o \
	$(OBJDIR)/ooxml/schemas.o \
	$(OBJDIR)/model/sheet.o

PSTRING_TEST_OBJFILES= \
	$(OBJDIR)/pstring.o \
	$(OBJDIR)/pstring_intern_test.o

DEPENDS= \
	$(ODF_HEADERS) \
	$(XLSX_HEADERS)

all: $(EXECS)

$(OBJDIR)/pre:
	mkdir -p $(OBJDIR)/model
	mkdir -p $(OBJDIR)/odf
	mkdir -p $(OBJDIR)/ooxml
	$(BINDIR)/gen-odf-tokens.py $(ODF_SCHEMAPATH) $(INCDIR)/odf/odf_token_constants.inl $(SRCDIR)/odf/odf_tokens.inl $(SRCDIR)/odf/odf_tokens.txt
	$(BINDIR)/gen-ooxml-tokens.py -t ooxml $(OOXML_SCHEMAPATH) \
		$(INCDIR)/ooxml/ooxml_token_constants.inl $(SRCDIR)/ooxml/ooxml_tokens.inl $(SRCDIR)/ooxml/ooxml_tokens.txt
	$(BINDIR)/gen-ooxml-tokens.py -t opc $(OPC_SCHEMAPATH) \
		$(INCDIR)/ooxml/opc_token_constants.inl $(SRCDIR)/ooxml/opc_tokens.inl $(SRCDIR)/ooxml/opc_tokens.txt
	touch $@

$(OBJDIR)/orcus_ods.o: $(SRCDIR)/orcus_ods.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/orcus_ods.cpp

$(OBJDIR)/orcus_xlsx.o: $(SRCDIR)/orcus_xlsx.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/orcus_xlsx.cpp

$(OBJDIR)/global.o: $(SRCDIR)/global.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/global.cpp

$(OBJDIR)/xml_context.o: $(SRCDIR)/xml_context.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xml_context.cpp

$(OBJDIR)/xml_handler.o: $(SRCDIR)/xml_handler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xml_handler.cpp

$(OBJDIR)/xml_simple_handler.o: $(SRCDIR)/xml_simple_handler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xml_simple_handler.cpp

$(OBJDIR)/xml_parser.o: $(SRCDIR)/xml_parser.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/xml_parser.cpp

$(OBJDIR)/tokens.o: $(SRCDIR)/tokens.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/tokens.cpp

$(OBJDIR)/pstring.o: $(SRCDIR)/pstring.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/pstring.cpp

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

$(OBJDIR)/ooxml/content_types.o: $(SRCDIR)/ooxml/content_types.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/content_types.cpp

$(OBJDIR)/ooxml/schemas.o: $(SRCDIR)/ooxml/schemas.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/schemas.cpp

$(OBJDIR)/ooxml/global.o: $(SRCDIR)/ooxml/global.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/global.cpp

$(OBJDIR)/ooxml/ooxml_tokens.o: $(SRCDIR)/ooxml/ooxml_tokens.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/ooxml_tokens.cpp

$(OBJDIR)/ooxml/opc_context.o: $(SRCDIR)/ooxml/opc_context.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/opc_context.cpp

$(OBJDIR)/ooxml/xlsx_handler.o: $(SRCDIR)/ooxml/xlsx_handler.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/xlsx_handler.cpp

$(OBJDIR)/ooxml/xlsx_context.o: $(SRCDIR)/ooxml/xlsx_context.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/ooxml/xlsx_context.cpp

# pstring intern test
$(OBJDIR)/pstring_intern_test.o: $(SRCDIR)/pstring_intern_test.cpp $(INCDIR)/pstring.hpp
	$(CXX) $(CPPFLAGS) -c $(SRCDIR)/pstring_intern_test.cpp -o $@

# model directory

$(OBJDIR)/model/sheet.o: $(SRCDIR)/model/sheet.cpp $(DEPENDS)
	$(CXX) $(CPPFLAGS) -c -o $@ $(SRCDIR)/model/sheet.cpp

orcus-ods: $(OBJDIR)/pre $(ODF_OBJFILES)
	$(CXX) $(LDFLAGS) $(ODF_OBJFILES) -o $@

orcus-xlsx: $(OBJDIR)/pre $(XLSX_OBJFILES)
	$(CXX) $(LDFLAGS) $(XLSX_OBJFILES) -o $@

pstring-intern-test: $(OBJDIR)/pre $(PSTRING_TEST_OBJFILES)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $(PSTRING_TEST_OBJFILES) -o $@

test.pstring: pstring-intern-test
	./pstring-intern-test

test.pstring.mem: pstring-intern-test
	valgrind --tool=memcheck --leak-check=full ./pstring-intern-test

test.ods: orcus-ods
	./orcus-ods ./test/test.ods

test.ou: orcus-ods
	./orcus-ods ./test/george-ou-perf.ods

test.xlsx: orcus-xlsx
	./orcus-xlsx ./test/test.xlsx

test.xlsx.mem: orcus-xlsx
	valgrind --tool=memcheck --leak-check=full ./orcus-xlsx ./test/test.xlsx

clean:
	rm -rf $(OBJDIR) 2> /dev/null || /bin/true
	rm $(EXECS) 2> /dev/null || /bin/true

