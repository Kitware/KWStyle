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
bool Parser::CheckStatementPerLine(unsigned long max,bool checkInlineFunctions)
{
  m_TestsDone[STATEMENTPERLINE] = true;
  char* val = new char[255];
  sprintf(val,"Statements per line = %ld max",max);
  m_TestsDescription[STATEMENTPERLINE] = val;
  delete [] val;

  if(!checkInlineFunctions)
    {
    m_TestsDescription[STATEMENTPERLINE] += " (not checking inline functions)";
    }

  bool hasError = false;
  size_t posSemicolon = m_BufferNoComment.find(";",0);
  long int currentLine = 0;
  unsigned long statements = 0;
  bool newline = false;
  std::string line = "";

  while(posSemicolon != std::string::npos)
    {
    // If we are on the same line
    long int lineNumber = this->GetLineNumber(posSemicolon,true);

    if(lineNumber != currentLine)
      {
      currentLine = lineNumber;
      statements = 0;
      }

    line = this->GetLine(lineNumber-1);

    // We need more than the current line to determine if we are between parenthesis
    // This is arbitrary but should work in most cases. Maybe the best will be to find the
    // previous semicolon but it might take a long time
    long int nb = static_cast<long int>(posSemicolon)-100; // 100 characters befor
    if(nb < 0)
      {
      nb = 0;
      }

    size_t ne = posSemicolon+100; // 100 characters after
    if(ne > m_BufferNoComment.size())
      {
      ne = m_BufferNoComment.size();
      }

    size_t posInLine2 = posSemicolon-nb;

    std::string line2 = m_BufferNoComment.substr(nb,ne-nb);

    if((!this->IsBetweenChars('(',')',posInLine2,false,line2))
      && (!this->IsBetweenQuote(posInLine2,false,line2))
      && line.find("case:") == std::string::npos // we allow switch/case to be in one line
      )
      {
      statements++;
      }

    size_t posSemicolon2 = m_BufferNoComment.find(";",posSemicolon+1);
    if(this->GetLineNumber(posSemicolon2,true) == currentLine)
      {
      newline = false;
      }
    else
      {
      newline = true;
      }

    bool reportError=true;

    if(!checkInlineFunctions)
      {
      if(this->IsInFunction(posSemicolon,m_BufferNoComment.c_str())
         && this->IsInClass(posSemicolon)
        )
        {
        reportError = false;
        }
      }

    if(reportError && statements > max && (newline || posSemicolon2==std::string::npos))
      {
      Error error;
      error.line = this->GetLineNumber(posSemicolon,true);
      error.line2 = error.line;
      error.number = STATEMENTPERLINE;
      error.description = "Number of statements per line exceed: ";
      char* localval = new char[10];
      sprintf(localval,"%ld",statements);
      error.description += localval;
      error.description += " (max=";
      delete [] localval;
      localval = new char[10];
      sprintf(localval,"%ld",max);
      error.description += localval;
      error.description += ")";
      delete [] localval;
      m_ErrorList.push_back(error);
      hasError = true;
      }

    posSemicolon = posSemicolon2;
    }

  return !hasError;
}

} // end namespace kws
