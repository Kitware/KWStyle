/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckSemicolonSpace.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of space between the end of the declaration
 *  and the semicolon */
bool Parser::CheckSemicolonSpace(unsigned long max)
{
  m_TestsDone[SEMICOLON_SPACE] = true;
  constexpr size_t length = 255;
  char* val = new char[length];
  snprintf(val,length,"Semicolons = %ld max spaces",max);
  m_TestsDescription[SEMICOLON_SPACE] = val;
  delete [] val;

  bool hasError = false;
  auto posSemicolon = static_cast<long int>(m_BufferNoComment.find(";", 0));
  while(posSemicolon != -1)
    {
    // We try to find the word before that
    long i=posSemicolon-1;
    unsigned long space = 0;
    while(i>=0)
      {
       if(m_BufferNoComment[i] == ' ')
        {
        if(!IsBetweenQuote(i))
          {
          space++;
          }
        }
       else
        {
        if(m_BufferNoComment[i] != '\n' && space > max)
          {
          Error error;
          error.line = this->GetLineNumber(posSemicolon,true);
          error.line2 = error.line;
          error.number = SEMICOLON_SPACE;
          error.description = "Number of spaces before semicolon exceed: ";
          constexpr size_t length = 10;
          char* localval = new char[length];
          snprintf(localval,length,"%ld",space);
          error.description += localval;
          error.description += " (max=";
          delete [] localval;
          localval = new char[length];
          snprintf(localval,length,"%ld",max);
          error.description += localval;
          error.description += ")";
          delete [] localval;
          m_ErrorList.push_back(error);
          hasError = true;

          if(m_FixFile)
            {
            this->ReplaceCharInFixedBuffer(this->GetPositionWithComments(posSemicolon-space),space,"");
            }
          } // space > max
        break;
        }
      i--;
      }
    posSemicolon = static_cast<long int>(m_BufferNoComment.find(";",posSemicolon+1));
    }

  // We also check if we have two or more semicolons following each other
  posSemicolon = static_cast<long int>(m_BufferNoComment.find(";",0));
  while(posSemicolon != -1)
    {
    // We look forward
    unsigned long i=posSemicolon+1;
    while(i<m_BufferNoComment.size())
      {
      if(m_BufferNoComment[i] == ' ' || m_BufferNoComment[i] == '\r' || m_BufferNoComment[i] == '\n')
        {
        // this is normal
        }
      else if(m_BufferNoComment[i] == ';')
        {
        // if we have for ( ;; ) we don't report
        // or ";;"
        if(!this->IsBetweenCharsFast('(',')',i)
           && !this->IsBetweenQuote(i))
          {
          Error error;
          error.line = this->GetLineNumber(i,true);
          error.line2 = error.line;
          error.number = SEMICOLON_SPACE;
          error.description = "Too many semicolons";
          m_ErrorList.push_back(error);
          hasError = true;

          if(m_FixFile)
            {
            this->ReplaceCharInFixedBuffer(this->GetPositionWithComments(i),1,"");
            }
          }
        }
      else
        {
        break;
        }
      i++;
      }
    posSemicolon = static_cast<long int>(m_BufferNoComment.find(";",i+1));
    }

  return !hasError;
}

} // end namespace kws
