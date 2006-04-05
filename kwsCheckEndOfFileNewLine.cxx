/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckEndOfFileNewLine.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the end of the file has a new line */
bool Parser::CheckEndOfFileNewLine()
{
  m_TestsDone[EOF_NEW_LINE] = true;
  char* val = new char[255];
  sprintf(val,"The file should have only one new line");
  m_TestsDescription[EOF_NEW_LINE] = val;
  delete [] val;
  bool hasError = false;
  
  // Check if the last character is an end of line
  if(m_Buffer[m_Buffer.size()-1] != '\n')
    {
    Error error;
    error.line = this->GetLineNumber(m_Buffer.size()-1,false);
    error.line2 = error.line;
    error.number = EOF_NEW_LINE;
    error.description = "No new line at the end of file";
    m_ErrorList.push_back(error);
    hasError = true;
    }

  // Check the number empty lines at the end of the file
  if((m_Buffer[m_Buffer.size()-1] == ' ') || (m_Buffer[m_Buffer.size()-1] == '\n')) 
    {
    long i = m_Buffer.size()-1;
    unsigned long numberOfEmptyLines = 0;
    while( ((m_Buffer[i] == '\n') ||  (m_Buffer[i] == ' ') || (m_Buffer[i] == '\r')) && (i>0))
      {
      if(m_Buffer[i] == '\n')
        {
        numberOfEmptyLines++;
        }
      i-=1;
      }
    
    if(numberOfEmptyLines>1)
      {
      // Maybe should be info and not error
      Error info;
      info.line2 = this->GetLineNumber(m_Buffer.size()-1,false)+1;
      info.line = info.line2-numberOfEmptyLines+2;      
      info.number = EOF_NEW_LINE;
      info.description = "Number of empty lines at the end of files: ";
      char* val = new char[10];
      sprintf(val,"%d",numberOfEmptyLines);
      info.description += val;
      delete [] val;
      m_ErrorList.push_back(info);
      }
    }

  return !hasError;
}

} // end namespace kws
