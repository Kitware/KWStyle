/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckTabs.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

#include "kwsPushBoostWarnings.h"
#include <boost/xpressive/xpressive.hpp>
#include "kwsPopBoostWarnings.h"

using namespace boost::xpressive;

namespace kws {

/** Check if the file contains tabs */
bool Parser::CheckTabs()
{
  m_TestsDone[TABS] = true;
  m_TestsDescription[TABS] = "The file should not have any tabs";

  bool hasError = false;

  sregex lineWithTabRegex = sregex::compile(R"(^[^\r\n]*?(\t)[^\r\n]*$)");
  sregex_iterator currentTabIterator(m_Buffer.begin(), m_Buffer.end(), lineWithTabRegex);
  sregex_iterator end;

  for (; currentTabIterator != end; ++currentTabIterator)
    {
    Error error;
    smatch const &currentMatch = *currentTabIterator;
    error.line   = this->GetLineNumber(currentMatch.position(1), false); 
    error.line2  = error.line;
    error.number = TABS;
    error.description = "Tabs identified";
    m_ErrorList.push_back(error);    
    hasError = true;
    }

  return !hasError;
}
} // end namespace kws
