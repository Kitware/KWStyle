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

  // We loop through the classes
  size_t classPosBegin = this->GetClassPosition(0);
  while(classPosBegin != std::string::npos)
    {
    // First we check in the public area
    size_t publicFirst;
    size_t publicLast;
    this->FindPublicArea(publicFirst,publicLast,classPosBegin);

    size_t previousline = 0;
    size_t previouspos = 0;

    size_t pos = publicFirst;
    while(pos!= std::string::npos)
      {
      std::string var = this->FindInternalVariable(pos+1,publicLast,pos);
      if (var.empty()) {
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
          unsigned long posvar = static_cast<unsigned long>(m_BufferNoComment.find(var,pos-var.size()-2));
          unsigned long l = static_cast<unsigned long>(this->GetPositionInLine(posvar));
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
          error.description = "Internal variable (" + var + ") doesn't match regular expression (";
          error.description += regEx;
          error.description += ")";
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }

    // Second in the protected area
    size_t protectedFirst;
    size_t protectedLast;
    this->FindProtectedArea(protectedFirst,protectedLast);
    pos = protectedFirst;

    previousline = 0;
    previouspos = 0;

    while(pos!=std::string::npos)
      {
      std::string var = this->FindInternalVariable(pos+1,protectedLast,pos);

      if (var.empty()) {
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
          size_t posvar = m_BufferNoComment.find(var,pos-var.size()-2);
          size_t l = this->GetPositionInLine(posvar);
          size_t line = this->GetLineNumber(pos,true);

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
          error.description = "Internal variable (" + var + ") doesn't match regular expression (";
          error.description += regEx;
          error.description += ")";
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }

    // Third and last in the private area
    size_t privateFirst;
    size_t privateLast;
    this->FindPrivateArea(privateFirst,privateLast);
    pos = privateFirst;
    previousline = 0;
    previouspos = 0;
    while(pos != std::string::npos)
      {
      std::string var = this->FindInternalVariable(pos+1,privateLast,pos);
      if (var.empty()) {
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
          size_t posvar = m_BufferNoComment.find(var,pos-var.size()-2);
          size_t l = this->GetPositionInLine(posvar);
          size_t line = this->GetLineNumber(pos,true);

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
          error.description = "Internal variable (" + var + ") doesn't match regular expression (";
          error.description += regEx;
          error.description += ")";
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }

    classPosBegin = this->GetClassPosition(classPosBegin+1);

    } // End loop class pos

  return !hasError;
}

/** Find the first ivar in the source code */
std::string Parser::FindInternalVariable(size_t start, size_t end,size_t & pos)
{
  size_t posSemicolon = m_BufferNoComment.find(";",start);
  while(posSemicolon != std::string::npos && posSemicolon<end)
    {
    // We try to find the word before that
    unsigned long i=static_cast<unsigned long>(posSemicolon)-1;
    bool inWord = true;
    bool first = false;
    std::string ivar = "";
    while(i != std::string::npos && inWord)
      {
      if((m_BufferNoComment[i] != ' ')
         && (m_BufferNoComment[i] != '\t')
        )
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
    while(i != std::string::npos)
      {
      if(m_BufferNoComment[i] == ';'
        || m_BufferNoComment[i] == '}'
        )
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

    bool isenum = false;
    // If we have a '}' we check that this is not an enum definition
    // or a typedef
    if(m_BufferNoComment[i] == '}')
      {

      // Find the opening char
      size_t j = this->FindOpeningChar('}','{',i,true);
      // Find a semicolon before that
      while(j>0)
        {
        if(m_BufferNoComment[j] == ';')
          {
          break;
          }
        j--;
        }

      if(m_BufferNoComment.substr(j+1,i-j-1).find("enum") != std::string::npos)
        {
        isenum = true;
        }
      else if(m_BufferNoComment.substr(j+1,i-j-1).find("typedef") != std::string::npos)
        {
        isenum = true;
        }
     }

    std::string subphrase = "";
    subphrase = m_BufferNoComment.substr(i+1,posSemicolon-i-1);

    if( (subphrase.find("=") == std::string::npos)
      && (subphrase.find("(") == std::string::npos)
      && (subphrase.find("typedef") == std::string::npos)
      && (subphrase.find("}") == std::string::npos)
      && (subphrase.find("friend") == std::string::npos)
      && (subphrase.find("class") == std::string::npos)
      && (subphrase.find("return") == std::string::npos)
      && (subphrase.find("extern") == std::string::npos)
      && (subphrase.find("\"") == std::string::npos)
      && (subphrase.find("<<") == std::string::npos)
      && !isenum
      )
      {
      // Check that we are not inside a function(){}
      if(!this->IsInFunction(posSemicolon))
        {
        // We check if any * is present and strip
        // the work
        size_t posstar = ivar.find_last_of("*");
        if(posstar != std::string::npos)
          {
          ivar = ivar.substr(posstar+1,ivar.size()-posstar-1);
          }
        return ivar;
        }
      }
    posSemicolon = m_BufferNoComment.find(";",posSemicolon+1);
    }

  pos = std::string::npos;
  return "";
}

} // end namespace kws
