/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckInternalVariables.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {



/** Check if the internal variables of the class are correct */
bool Parser::CheckInternalVariables(const char* regEx,bool alignment,bool checkProtected)
{
  if(checkProtected)
    {
    m_TestsDone[IVAR_PUBLIC] = true;
    m_TestsDescription[IVAR_PUBLIC] = "ivars should be in the protected or private section";
    }

  m_TestsDone[IVAR_REGEX] = true;
  m_TestsDescription[IVAR_REGEX] = "ivars should match regular expression: ";
  m_TestsDescription[IVAR_REGEX] += regEx;

  if(alignment)
    {
    m_TestsDone[IVAR_ALIGN] = true;
    m_TestsDescription[IVAR_ALIGN] = "ivars should be aligned with previous ivars ";
    }

  // First we need to find the parameters
  // float myParam;
  bool hasError = false;

  kwssys::RegularExpression regex(regEx);

  // First we check in the public area
  long int publicFirst;
  long int publicLast;
  this->FindPublicArea(publicFirst,publicLast);

  long int previousline = 0;
  long int previouspos = 0;
  
  long int pos = publicFirst;
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,publicLast,pos);
    if(var == "")
      {
      continue;
      }

    if(this->IsInStruct(pos) || this->IsInUnion(pos))
      {
      continue;
      }

    if(var.length() > 0)
      {
      if(checkProtected)
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = IVAR_PUBLIC;
        error.description = "Encapsulation not preserved";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      
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
          if(l!=previouspos)
            {
            Error error;
            error.line = this->GetLineNumber(pos,true);
            error.line2 = error.line;
            error.number = IVAR_ALIGN;
            error.description = "Internal variable (" + var + ") is not aligned with the previous one";
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
        error.number = IVAR_REGEX;
        error.description = "Internal variable (" + var + ") doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }

  // Second in the protected area
  long int protectedFirst;
  long int protectedLast;
  this->FindProtectedArea(protectedFirst,protectedLast);
  pos = protectedFirst;

  previousline = 0;
  previouspos = 0;

  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,protectedLast,pos);

    if(var == "")
      {
      continue;
      }
 
    if(this->IsInStruct(pos) || this->IsInUnion(pos))
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
          if(l!=previouspos)
            {
            Error error;
            error.line = this->GetLineNumber(pos,true);
            error.line2 = error.line;
            error.number = IVAR_ALIGN;
            error.description = "Internal variable (" + var + ") is not aligned with the previous one";
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
        error.number = IVAR_REGEX;
        error.description = "Internal variable (" + var + ") doesn't match regular expression.";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }

  // Third and last in the private area
  long int privateFirst;
  long int privateLast;
  this->FindPrivateArea(privateFirst,privateLast);
  pos = privateFirst;
  previousline = 0;
  previouspos = 0;
  while(pos != -1)
    {
    std::string var = this->FindInternalVariable(pos+1,privateLast,pos); 
    if(var == "")
      {
      continue;
      }
    
    if(this->IsInStruct(pos) || this->IsInUnion(pos))
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
          if(l!=previouspos)
            {
            Error error;
            error.line = this->GetLineNumber(pos,true);
            error.line2 = error.line;
            error.number = IVAR_ALIGN;
            error.description = "Internal variable (" + var + ") is not aligned with the previous one";
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
        error.number = IVAR_REGEX;
        error.description = "Internal variable(" + var + ") doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }

  return !hasError;
}

/** Find the first ivar in the source code */
std::string Parser::FindInternalVariable(long int start, long int end,long int & pos)
{
  long int posSemicolon = m_BufferNoComment.find(";",start);
  while(posSemicolon != -1 && posSemicolon<end)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    bool inWord = true;
    bool first = false;
    std::string ivar = "";
    while(i>=0 && inWord)
      {
      if(m_BufferNoComment[i] != ' ')
        {
        if((m_BufferNoComment[i] == '}')
          || (m_BufferNoComment[i] == ')')
          || (m_BufferNoComment[i] == ']')
          || (m_BufferNoComment[i] == '\n')
          )
          {
          inWord = false;
          }
        else
          {
          std::string store = ivar;
          ivar = m_BufferNoComment[i];
          ivar += store;
          inWord = true;
          first = true;
          }
        }
      else // we have a space
        {
        if(first)
          {
          inWord = false;
          }
        }
      i--;
      }
    pos = posSemicolon;
    // We extract the complete definition.
    // This means that we look for a '{' or '}' or '{' or ':'
    // but not '::'
    while(i>0)
      {
      if(m_BufferNoComment[i] == ';')
        {
        break;
        }
      else if(m_BufferNoComment[i] == ':')
        {
        if((m_BufferNoComment[i-1] != ':') && (m_BufferNoComment[i+1] != ':'))
          {
          break;
          }
        }
      i--;
      }

    std::string subphrase = "";

    if(i>=0)
      {
      subphrase = m_BufferNoComment.substr(i+1,posSemicolon-i-1);
      }

    if( (subphrase.find("=") == -1)
      && (subphrase.find("(") == -1)
      && (subphrase.find("typedef") == -1)
      && (subphrase.find("}") == -1)
      && (subphrase.find("friend") == -1)
      && (subphrase.find("class") == -1)
      && (subphrase.find("return") == -1)
      && (subphrase.find("\"") == -1)
      && (subphrase.find("<<") == -1)
      )
      {
      // Check that we are not inside a function(){}
      if(!this->IsInFunction(posSemicolon))
        {
        // If the ivar start with a * we remove it
        if(ivar[0] == '*')
          {
          ivar = ivar.substr(1,ivar.size()-1);
          }
        std::cout << ivar.c_str() << std::endl;
        return ivar;
        }
      }
    posSemicolon = m_BufferNoComment.find(";",posSemicolon+1);
    }

  pos = -1;
  return "";
}

} // end namespace kws
