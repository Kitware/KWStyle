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
    char* temp = new char[10];
    sprintf(temp,"%ld",maxLength);        
    m_TestsDescription[FUNCTION_LENGTH] += temp;
    m_TestsDescription[FUNCTION_LENGTH] += " lines";
    delete [] temp;
    }

  // First we need to find the parameters
  bool hasError = false;

  kwssys::RegularExpression regex(regEx);

  // List all the function in the file
  long int pos = this->FindFunction(0);
  while(pos != -1)
    {
    // We extract the name of the function
    std::string functionName = "";
    // Find the ) and the openning (
    long int i=pos;
    for(;i>0;i--)
      {
      if(m_BufferNoComment[i] == ')')
        {
        i = this->FindOpeningChar(')','(',i,true);
        i--;
        break;
        }
      }

    bool inWord = false;
    for(;i>0;i--)
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

    if(functionLine.find("#define") == std::string::npos
       && functionLine.find("_attribute_") == std::string::npos
       && functionLine.find(" operator") == std::string::npos
       && functionLine.find("friend ") == std::string::npos
       && functionName.find("if") == std::string::npos
       && functionName.find("while") == std::string::npos
       && functionName.find("for") == std::string::npos
       && functionName.find("switch") == std::string::npos
       && functionName.find("main") == std::string::npos
       && functionName.find("~") == std::string::npos // skip destructor for now...
       )
      {
      long int posf = functionName.find("::",0);
      long int posp = functionName.find("(",posf);
      if(posp != -1 && posf != -1 && posp>posf)
        {
        functionName = functionName.substr(posf+2,posp-posf-2);
        }
      else if(posf != -1)
        {
        functionName = functionName.substr(posf+2,functionName.size()-posf-2);
        }
      }
    else
      {
      functionName = "";
      }

    //std::cout << "Function Name = " << functionName.c_str() << std::endl;

    if(functionName.size() == 0)
      {
      long int bf = m_BufferNoComment.find('{',pos);
      long int pos2 = this->FindClosingChar('{','}',bf,true);
      pos = this->FindFunction(pos2+1);

      // we cannot go backward
      /*if(pos2 > pos)
        {
        long int bf = m_BufferNoComment.find('{',pos2);
        pos = this->FindFunction(bf);
        }*/
      continue;
      }
    else if(functionName.size()>0)
      {
      long int bf = m_BufferNoComment.find('{',pos);
      long int bfcomments = GetPositionWithComments(bf);
      long int bfl = this->GetLineNumber(bfcomments);
      long int pos2 = this->FindClosingChar('{','}',bf,true);
      long int poscomments = GetPositionWithComments(pos2);
      long int efl = this->GetLineNumber(poscomments);
      
      pos = this->FindFunction(pos2+1);

      // we cannot go backward
      if(pos2 > pos)
        {
        long int bf = m_BufferNoComment.find('{',pos2);
        pos = this->FindFunction(bf);
        }
     
      if(!regex.find(functionName))
        {
        Error error;
        error.line = bfl;
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
          error.line = bfl;
          error.line2 = efl;
          error.number = FUNCTION_LENGTH;
          error.description = "function (" + functionName + ") has too many lines: ";
          char* temp = new char[10];
          sprintf(temp,"%ld",efl-bfl);
          error.description += temp;
          error.description += " (";
          sprintf(temp,"%ld",maxLength);
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
