/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include "ooxml/ooxml_tokens.hpp"

namespace orcus {

namespace ooxml {

#include "ooxml_tokens.inl"

}

namespace opc {

#include "opc_tokens.inl"

}

tokens ooxml_tokens = 
    tokens(ooxml::token_names, ooxml::token_name_count, 
                ooxml::nstoken_names, ooxml::nstoken_name_count);

tokens opc_tokens = 
    tokens(opc::token_names, opc::token_name_count, 
                opc::nstoken_names, opc::nstoken_name_count);

}
