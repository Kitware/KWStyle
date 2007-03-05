/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckFunctions.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if ANY function implement in the given file is correct */
bool Parser::CheckFunctions(const char* regEx,unsigned long maxLength)
{
  if(regEx)
    {
    m_TestsDone[FUNCTION_REGEX] = true;
    m_TestsDescription[FUNCTION_REGEX] = "Functions should match regular expression: ";
    m_TestsDescription[FUNCTION_REGEX] += regEx;
    }

  if(maxLength>0)
    {
    m_TestsDone[FUNCTION_LENGTH] = true;
    m_TestsDescription[FUNCTION_LENGTH] = "Functions must not exceed: ";
    m_TestsDescription[FUNCTION_LENGTH] += maxLength;
    m_TestsDescription[FUNCTION_LENGTH] += " lines";
    }

  // First we need to find the parameters
  bool hasError = false;

  kwssys::RegularExpression regex(regEx);

  // List all the function in the file
  long int pos = this->FindFunction(0);

  while(pos != -1)
    {
    long int posf = pos;
    // We extract the name of the function
    std::string functionName = "";
    bool inWord = false;
    long int i=pos;
    for(i;i>0;i--)
      {
      if(m_BufferNoComment[i] != ' ' && m_BufferNoComment[i] != '\t' 
         && m_BufferNoComment[i] != '\r' && m_BufferNoComment[i] != '\n' 
         && m_BufferNoComment[i] != '*' && m_BufferNoComment[i] != '&')
        {
        inWord = true;
        functionName = m_BufferNoComment[i]+functionName;
        }
      else if(inWord)
        {
        break;
        }
      }
      
    // Check that this is not a #define (tricky)
    std::string functionLine = this->GetLine(this->GetLineNumber(i,true)-1);
    if(functionLine.find("#define") == -1
       && functionLine.find("_attribute_") == -1
       && functionLine.find(" operator") == -1
       && functionLine.find("friend ") == -1)
      {
      long int posf = functionName.find("::",0);
      long int posp = functionName.find("(",posf);
      if(posp != -1 && posf != -1 && posp>posf)
        {
        functionName = functionName.substr(posf+2,posp-posf-2);
        }
      }
    else
      {
      functionName = "";
      }

    if(functionName.size()>0)
      {
      long int bf = m_BufferNoComment.find('{',pos);
      long int bfl = this->GetLineNumber(bf);
      pos = this->FindClosingChar('{','}',bf);
      long int efl = this->GetLineNumber(pos);
      pos = this->FindFunction(pos+1);

      if(!regex.find(functionName))
        {
        Error error;
        error.line = this->GetLineNumber(posf,true);
        error.line2 = error.line;
        error.number = FUNCTION_REGEX;
        error.description = "function (" + functionName + ") doesn't match regular expression: " + regEx;
        m_ErrorList.push_back(error);
        hasError = true;
        }

      if(maxLength>0)
        {
        if((bfl>-1) && (efl>-1) && (efl-bfl>(long int)maxLength))
          {
          Error error;
          error.line = this->GetLineNumber(bfl,true);
          error.line2 = this->GetLineNumber(efl,true);
          error.number = FUNCTION_LENGTH;
          error.description = "function (" + functionName + ") has too many lines: ";
          char* temp = new char[10];
          sprintf(temp,"%d",efl-bfl);
          error.description += temp;
          error.description += " (";
          sprintf(temp,"%d",maxLength);
          error.description += temp;
          error.description += ")";
          m_ErrorList.push_back(error);
          hasError = true;
          delete [] temp; 
          }
        }
      }
    }
  return !hasError;
}

} // end namespace kws
