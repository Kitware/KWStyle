/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckBadCharacters.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

#include <boost/xpressive/xpressive.hpp>
using namespace boost::xpressive;

namespace kws {

/** Check if the current file as bad characters like δόφί*/
bool Parser::CheckBadCharacters(bool checkComments)
{
  m_TestsDone[BADCHARACTERS] = true;
  m_TestsDescription[BADCHARACTERS] = "Checking for bad characters";

  std::string buffer = m_BufferNoComment;
  if(checkComments)
    {
    buffer = m_Buffer;
    }
  
  bool hasError = false;

  // space characters along with printable characters (space through
  // ~) are fine; others are "bad"
  sregex lineWithBadCharRegex = sregex::compile("^[^\\r\\n]*?([^\\s[:print:]])[^\\r\\n]*$");
  sregex_iterator currentBadCharIterator(buffer.begin(), buffer.end(), lineWithBadCharRegex);
  sregex_iterator end;

  for (; currentBadCharIterator != end; ++currentBadCharIterator)
    {
    Error error;
    smatch const &currentMatch = *currentBadCharIterator;
    error.line   = this->GetLineNumber(currentMatch.position(1), !checkComments); 
    error.line2  = error.line;
    error.number = BADCHARACTERS;
    error.description = "Bad character";
    m_ErrorList.push_back(error);    
    hasError = true;
    }

  return !hasError;
}

} // end namespace kws
