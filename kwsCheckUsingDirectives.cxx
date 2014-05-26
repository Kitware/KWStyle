/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckUsingDirectives.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the file contains "using namespace .* ;" */
bool Parser::CheckUsingDirectives(bool forbidUsingDirectives)
{
  if(!forbidUsingDirectives)
    return true;

  bool hasError = false;
  const char * ptr = m_BufferNoComment.c_str();
  kwssys::RegularExpression regex("using[ \r\n\t]+namespace[ \r\n\t]+[A-Za-z_]+[ \r\n\t]*;");
  while(ptr && regex.find(ptr))
    {
    ptr = m_BufferNoComment.c_str() + regex.end();
    if(this->IsBetweenQuote(regex.start(), false))
      continue;

    hasError = true;
    Error error;
    error.line = this->GetLineNumber(regex.start(), true);
    error.line2 = error.line;
    error.description = "Using namespace directive used.";
    error.number = USING_DIRECTIVES;
    m_ErrorList.push_back(error);
    }
  return !hasError;
}

} // end namespace kws
