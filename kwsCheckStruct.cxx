/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckStruct.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check if the struct variables of the class are correct */
bool Parser::CheckStruct(const char* regEx,bool alignment)
{
  m_TestsDone[SVAR_REGEX] = true;
  m_TestsDescription[SVAR_REGEX] = "struct variables should match regular expression: ";
  m_TestsDescription[SVAR_REGEX] += regEx;

  if(alignment)
    {
    m_TestsDone[SVAR_ALIGN] = true;
    m_TestsDescription[SVAR_ALIGN] = "struct variables should be aligned with previous vars ";
    }

  // First we need to find the parameters
  // float myParam;
  bool hasError = false;

  kwssys::RegularExpression regex(regEx);

  // find the struct

  size_t posStruct = m_BufferNoComment.find("struct",0);

  while(posStruct != -1)
    {
    size_t begin = posStruct;
    while(posStruct<(size_t)m_BufferNoComment.size())
      {
      if(m_BufferNoComment[posStruct] == '{')
        {
        break;
        }
      posStruct++;
      }
    size_t end = this->FindClosingChar('{','}',posStruct,true);

    size_t previousline = 0;
    size_t previouspos = 0;
    
    size_t pos = begin;
    while(pos!= -1)
      {
      std::string var = this->FindInternalVariable(pos+1,end+1,pos);
      if(var == "")
        {
        continue;
        }

      if(var.length() > 0)
        {   
        // Check the alignment if specified
        if(alignment)
          {
          // Find the position in the line
          unsigned long posvar = m_BufferNoComment.find(var,pos-var.size()-2);
          unsigned long l = this->GetPositionInLine(posvar);
          unsigned long line = this->GetLineNumber(pos,true);

          // if the typedef is on a line close to the previous one we check
          if(line-previousline<2)
            {
            if(l!=static_cast<unsigned long>(previouspos))
              {
              Error error;
              error.line = this->GetLineNumber(pos,true);
              error.line2 = error.line;
              error.number = SVAR_ALIGN;
              error.description = "Struct variable (" + var + ") is not aligned with the previous one";
              m_ErrorList.push_back(error);
              hasError = true;
              }
            }
          else
            {
            previouspos = l;
            }
          previousline = line;
          } // end alignement

        if(!regex.find(var))
          {
          Error error;
          error.line = this->GetLineNumber(pos,true);
          error.line2 = error.line;
          error.number = SVAR_REGEX;
          error.description = "Struct variable (" + var + ") doesn't match regular expression";
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }
    posStruct = m_BufferNoComment.find("struct",posStruct+1);
    } // end struct loop

  return !hasError;
}

} // end namespace kws
