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
#include <cctype>

namespace kws {

/** Check if the file contains "using namespace .* ;" */
bool Parser::CheckUsingDirectives(bool forbidUsingDirectives)
{
  if(!forbidUsingDirectives)
    return true;

  bool hasError = false;

  size_t usingPos = m_BufferNoComment.find("using");
  while( usingPos != std::string::npos )
    {
    size_t tempIndex = usingPos + 5;//points right after "using"
    if(this->IsBetweenQuote(usingPos, false))
      {
      usingPos = m_BufferNoComment.find("using", tempIndex);
      continue;
      }

    //Munch whitespace (at least one character)
    int whitespaceCharacterCount = 0;
    while(tempIndex < m_BufferNoComment.size() && isspace(m_BufferNoComment[tempIndex]))
      {
      tempIndex++;
      whitespaceCharacterCount++;
      }
    if( tempIndex >= m_BufferNoComment.size() || whitespaceCharacterCount < 1 )//file ended or not enough whitespace characters
      return true;

    //Munch "namespace"
    if(m_BufferNoComment.substr(tempIndex, 9) != "namespace")
      {
      usingPos = m_BufferNoComment.find("using", tempIndex);
      continue;
      }
    tempIndex += 9;//points right after "namespace"

    //Munch whitespace (at least one character)
    whitespaceCharacterCount = 0;
    while(tempIndex < m_BufferNoComment.size() && isspace(m_BufferNoComment[tempIndex]))
      {
      tempIndex++;
      whitespaceCharacterCount++;
      }
    if( tempIndex >= m_BufferNoComment.size() || whitespaceCharacterCount < 1 )//file ended or not enough whitespace characters
      return true;

    //Munch identifier
    while(tempIndex < m_BufferNoComment.size() && (isalpha(m_BufferNoComment[tempIndex]) || m_BufferNoComment[tempIndex] == '_') )
      tempIndex++;
    if( tempIndex >= m_BufferNoComment.size() )//file ended
      return true;

    //Munch whitespace
    while(tempIndex < m_BufferNoComment.size() && isspace(m_BufferNoComment[tempIndex]))
      tempIndex++;
    if( tempIndex >= m_BufferNoComment.size() )//file ended
      return true;

    if(m_BufferNoComment[tempIndex] == ';')//Match found for the full "using namespace [A-Za-z_]+;"
      {
      hasError = true;
      Error error;
      error.line = this->GetLineNumber(usingPos, true);
      error.line2 = error.line;
      error.description = "Using namespace directive used.";
      error.number = USING_DIRECTIVES;
      m_ErrorList.push_back(error);
      }

    usingPos = m_BufferNoComment.find("using", tempIndex);
    }

  return !hasError;
}

} // end namespace kws
