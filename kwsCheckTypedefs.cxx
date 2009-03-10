/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckTypedefs.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the typedefs of the class are correct */
bool Parser::CheckTypedefs(const char* regEx, bool alignment,unsigned int maxLength)
{
  if(alignment)
    {
    m_TestsDone[TYPEDEF_ALIGN] = true;
    char* val = new char[255];
    sprintf(val,"Typedefs should be aligned");
    m_TestsDescription[TYPEDEF_ALIGN] = val;
    delete [] val;
    }

  m_TestsDone[TYPEDEF_REGEX] = true;
  m_TestsDescription[TYPEDEF_REGEX] = "Typedefs should match regular expression: ";
  m_TestsDescription[TYPEDEF_REGEX] += regEx;

  // First we need to find the typedefs
  // typedef type MyTypeDef;
  bool hasError = false;

  kwssys::RegularExpression regex(regEx);

  size_t previousline = 0;
  size_t previouspos = 0;
  size_t pos = 0;
  while(pos!= std::string::npos)
    {
    size_t beg = 0;
    size_t typedefpos = 0;
    std::string var = this->FindTypedef(pos+1,m_BufferNoComment.size(),pos,beg,typedefpos);
    
    if(var == "")
      {
      continue;
      }
    // Check the alignment if specified
    if(alignment)
      {
      // Find the position in the line
      size_t l = this->GetPositionInLine(beg);
      size_t line = this->GetLineNumber(beg,true);
      size_t typdefline = this->GetLineNumber(typedefpos,true);

      // if the typedef is on a line close to the previous one we check
      if(typdefline-previousline<2)
        {
        if(l!=previouspos)
          {
          bool reportError = true;
          // We check that the previous line is not ending with a semicolon
          // and that the sum of the two lines is more than maxLength
          std::string previousLine = this->GetLine(this->GetLineNumber(beg,true)-2);
          std::string currentLine = this->GetLine(this->GetLineNumber(beg,true)-1);
          if( (previousLine[previousLine.size()-1] != ';')
             && (previousLine.size()+currentLine.size()>maxLength)
            )
            {
            reportError = false;
            }

          // Check if the alignement is possible due to the length of the line
          size_t size = currentLine.size()-l;
          if(previouspos+size>maxLength)
            {
            previouspos = l;
            reportError = false;
            }

          // This one is a bit tricky. Check the minimum
          // typedef igstk::VascularNetworkObject   VascularNetworkObjectType;
          // typedef VascularNetworkObjectType::VesselObjectType VesselObjectType;
          // First find the optimal position (one space) for the current line
          size_t optimalCurrentLinePos = l;
          while(optimalCurrentLinePos>1 && currentLine[optimalCurrentLinePos-1] == ' ')
            {
            optimalCurrentLinePos--;
            }
          optimalCurrentLinePos++;
          // Second find the size of the previous typedefs;
          size_t sizeTypedef = previousLine.size()-previouspos;
          // if the size is too big we don't report
          if(optimalCurrentLinePos+sizeTypedef>maxLength)
            {
            previouspos = l;
            reportError = false;
            }

          if(reportError)
            {
            Error error;
            error.line = this->GetLineNumber(beg,true);
            error.line2 = error.line;
            error.number = TYPEDEF_ALIGN;
            error.description = "Type definition (" + var + ") is not aligned with the previous one: ";
            char* var = new char[10];
            sprintf(var,"%ld",l);
            error.description += var;
            error.description += " v.s. ";
            sprintf(var,"%ld",previouspos);
            error.description += var;
            delete [] var;
            m_ErrorList.push_back(error);
            hasError = true;
            }
          }
        }
      else
        {
        previouspos = l;
        }
      previousline = line;
      }

    if(!regex.find(var))
      {
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = TYPEDEF_REGEX;
      error.description = "Type definition (" + var + ") doesn't match regular expression";
      m_ErrorList.push_back(error);
      hasError = true;
      }
    }
  return !hasError;
}


/** Find a typedef in the source code */
std::string Parser::FindTypedef(size_t start, size_t end,size_t & pos,size_t & beg,size_t & typdefpos)
{
  size_t posTypedef  = m_BufferNoComment.find("typedef",start);
  if(posTypedef == std::string::npos)
    {
    pos = std::string::npos;
    return "";
    }

  typdefpos = posTypedef;
  size_t posSemicolon = m_BufferNoComment.find(";",posTypedef);

  // Check if we have any () in the subword then we don't check the typdef
  std::string sub = m_BufferNoComment.substr(posTypedef,posSemicolon-posTypedef);
  if((sub.find("(",0) != std::string::npos) 
    || (sub.find(")",0) != std::string::npos)
    || (sub.find("{",0) != std::string::npos)
    || (sub.find("}",0) != std::string::npos)
    )
    {
    pos = posSemicolon;
    return "";
    }

  std::string typedefname = "";
  if(posSemicolon != std::string::npos && posSemicolon<end)
    {
    // We try to find the word before that
    size_t i=posSemicolon-1;
    bool inWord = true;
    bool first = false;
    while(i>=0 && inWord)
      {
      if(m_BufferNoComment[i] != ' ')
        {
        std::string store = typedefname;
        typedefname = m_BufferNoComment[i];
        typedefname += store;
        beg = i;
        inWord = true;
        first = true;
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
    }

  pos = posSemicolon;
  return typedefname;
}

} // end namespace kws
