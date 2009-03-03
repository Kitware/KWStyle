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

namespace kws {

/** Check if the file contains tabs */
bool Parser::CheckTabs()
{
  m_TestsDone[TABS] = true;
  char* val = new char[255];
  sprintf(val,"The file should not have any tabs");
  m_TestsDescription[TABS] = val;
  delete [] val;

  bool hasError = false;
  size_t pos = m_Buffer.find('\t',0);    
  size_t line = 0;

  // Show only one tab per line
  while(pos != -1)
    {
    if(this->GetLineNumber(pos,false) != line)
      {
      line = this->GetLineNumber(pos,false);
      Error error;
      error.line = line; 
      error.line2 = error.line;
      error.number = TABS;
      error.description = "Tabs identified";
      m_ErrorList.push_back(error);
      }
    pos = m_Buffer.find('\t',pos+1);
    hasError = true;  
    }

  return !hasError;
}
} // end namespace kws
