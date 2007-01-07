/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckStatementPerLine.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of statements per line */
bool Parser::CheckStatementPerLine(unsigned long max)
{
  m_TestsDone[STATEMENTPERLINE] = true;
  char* val = new char[255];
  sprintf(val,"Statements per line = %d max",max);
  m_TestsDescription[STATEMENTPERLINE] = val;
  delete [] val;

  bool hasError = false;
  long int posSemicolon = m_BufferNoComment.find(";",0);
  long int currentLine = -1;
  unsigned long statements = 0;
  while(posSemicolon != -1)
    {
    // If we are on the same line
    if((this->GetLineNumber(posSemicolon,true) == currentLine)
      && (!this->IsBetweenChars('(',')',posSemicolon,false))
      )
      {
      statements++;
      }
    else
      {
      if(!this->IsBetweenChars('(',')',posSemicolon,false))
        {
        currentLine = this->GetLineNumber(posSemicolon,true);
        statements = 1;
        }
      }

    if(statements > max)
      {
      Error error;
      error.line = this->GetLineNumber(posSemicolon,true);
      error.line2 = error.line;
      error.number = STATEMENTPERLINE;
      error.description = "Number of statements per line exceed: ";
      char* val = new char[10];
      sprintf(val,"%d",statements);
      error.description += val;
      error.description += " (max=";
      delete [] val;
      val = new char[10];
      sprintf(val,"%d",max);
      error.description += val;
      error.description += ")";
      delete [] val;
      m_ErrorList.push_back(error);
      hasError = true;
      }
    posSemicolon = m_BufferNoComment.find(";",posSemicolon+1);
    }

  return !hasError;
}

} // end namespace kws
