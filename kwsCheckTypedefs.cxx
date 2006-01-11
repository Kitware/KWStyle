/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckTypedefs.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Author:    Julien Jomier

  Copyright (c) 2002 CADDLab @ UNC. All rights reserved.
  See itkUNCCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the typedefs of the class are correct */
bool Parser::CheckTypedefs(const char* regEx, bool alignment)
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

  char* val = new char[255];
  sprintf(val,"Typedefs should match regular expression: %s",regEx);
  m_TestsDescription[TYPEDEF_REGEX] = val;
  delete [] val;

  // First we need to find the typedefs
  // typedef type MyTypeDef;
  bool hasError = false;

  itksys::RegularExpression regex(regEx);

  long int previousline = 0;
  long int previouspos = 0;
  long int pos = 0;
  while(pos!= -1)
    {
    long int beg = 0;
    std::string var = this->FindTypedef(pos+1,m_BufferNoComment.size(),pos,beg);
    
    if(var == "")
      {
      continue;
      }
    // Check the alignment if specified
    if(alignment)
      {
      // Find the position in the line
      unsigned long l = this->GetPositionInLine(beg);
      unsigned long line = this->GetLineNumber(beg,false);
      
      // if the typedef is on a line close to the previous one we check
      if(line-previousline<=5)
        {
         if(l!=previouspos)
           {
           Error error;
           error.line = this->GetLineNumber(beg,true);
           error.line2 = error.line;
           error.number = TYPEDEF_ALIGN;
           error.description = "Type definition (" + var + ") is not aligned with the previous one";
           m_ErrorList.push_back(error);
           hasError = true;
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
std::string Parser::FindTypedef(long int start, long int end,long int & pos,long int & beg)
{
  long int posTypedef  = m_BufferNoComment.find("typedef",start);
  if(posTypedef == -1)
    {
    pos = -1;
    return "";
    }

  long int posSemicolon = m_BufferNoComment.find(";",posTypedef);

  std::string typedefname = "";
  if(posSemicolon != -1 && posSemicolon<end)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    bool inWord = true;
    bool first = false;
    while(i>=0 && inWord)
      {
      if(m_BufferNoComment[i] != ' ')
        {
        /*if((m_BufferNoComment[i] == '}')
          || (m_BufferNoComment[i] == ')')
          || (m_BufferNoComment[i] == ']')
          || (m_BufferNoComment[i] == '\n')
          )
          {
          inWord = false;
          }
        else
          {*/
          std::string store = typedefname;
          typedefname = m_BufferNoComment[i];
          typedefname += store;
          beg = i;
          inWord = true;
          first = true;
          //}
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
