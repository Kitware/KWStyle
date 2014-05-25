/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckIfNDefDefine.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include <cctype>

namespace kws {


/** Check if the #ifndef/#define is defined correctly for the class 
 *  match can contain [NameOfClass] and [Extension] */
bool Parser::CheckIfNDefDefine(const char* match, bool uppercaseTheDefinition)
{
  // Check only if we are not a .cxx or .cc or .c file
  if(m_Filename.find(".c") != std::string::npos)
    {
    return false;
    }

  m_TestsDone[NDEFINE] = true;
  char* val = new char[512];
  sprintf(val,"#ifndef/#define should match ");
  m_TestsDescription[NDEFINE] = val;
  // Replace < and >
  std::string l = match;
  long int inf = l.find("<",0);
  while(inf != -1)
    {
    l.replace(inf,1,"&lt;");
    inf = l.find("<",0);
    }

  long int sup = l.find(">",0);
  while(sup != -1)
    {
    l.replace(sup,1,"&gt;");
    sup = l.find(">",0);
    }
  m_TestsDescription[NDEFINE] += l;
  delete [] val;

  bool hasError = false;
  bool notDefined = false;


  // We should have a #ifndef/#define on the same line and we go
  // through the file until we find a correct match.
  bool doesMatch = true;

  // Find the #ifndef word in the file
  long int pos = m_BufferNoComment.find("#ifndef",0);
  if(pos == -1)
    {
    notDefined = true;
    }
  else
    {
    for(long int i=0;i<pos;i++)
      {
      if((m_BufferNoComment[i] != ' ')
        && (m_BufferNoComment[i] != '\r')
        && (m_BufferNoComment[i] != '\n')
        )
        {
        notDefined = true;
        }
      }
    }

  long int definepos = pos;

  if(notDefined)
    {
    Error error;
    error.line = this->GetLineNumber(0,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#ifndef not defined";
    m_ErrorList.push_back(error);
    return false;
    }

  while(!notDefined)
    {
    doesMatch = true;

    // Find the word after #ifndef
    pos += 8;
   
    while(m_BufferNoComment[pos] == ' ')
      {
      pos++;
      }
    long int begin = pos;
    while((m_BufferNoComment[pos] != ' ') 
      &&(m_BufferNoComment[pos] != '\r')
      &&(m_BufferNoComment[pos] != '\n')
      )
      {
      pos++;
      }
    long int end = pos;
    std::string ifndef = m_BufferNoComment.substr(begin,end-begin);

    // Find the word after #define
    long int posDef = m_BufferNoComment.find("#define",end);

    if(posDef == -1)
      {
      Error error;
      error.line = this->GetLineNumber(end,true);
      error.line2 = error.line;
      error.number = NDEFINE;
      error.description = "#define not defined";
      m_ErrorList.push_back(error);
      return false;
      }
    
    posDef += 7; 
    while(m_BufferNoComment[posDef] == ' ')
      {
      posDef++;
      }
    begin = posDef;
    while((m_BufferNoComment[posDef] != ' ') 
      &&(m_BufferNoComment[posDef] != '\r')
      &&(m_BufferNoComment[posDef] != '\n')
      )
      {
      posDef++;
      }
    end = posDef;
    std::string define = m_BufferNoComment.substr(begin,end-begin);

    if(ifndef != define)
      {
      doesMatch = false;
      }

    if(doesMatch)
      {
      if(m_Filename == "")
        {
        std::cout << "CheckIfNDefDefine() : m_Filename shoud be set" << std::endl;
        return false;
        }

      long int point = m_Filename.find_last_of(".");
      long int slash = m_Filename.find_last_of("/");

      std::string nameofclass = m_Filename.substr(slash+1,point-slash-1);
      std::string extension = m_Filename.substr(point+1,m_Filename.size()-point-1);

      // construct the string
      std::string toMatch = match;
      long int posnofc = toMatch.find("[NameOfClass]");
      if(posnofc != -1)
        {
        toMatch.replace(posnofc,13,nameofclass);
        }
      posnofc = toMatch.find("[Extension]");
      if(posnofc != -1)
        {
        toMatch.replace(posnofc,11,extension);
        }

      if(uppercaseTheDefinition)
        {
        for(std::string::iterator it = toMatch.begin(); it != toMatch.end(); it++)
          *it = std::toupper(*it);
        }

      if(ifndef != toMatch)
        {   
        Error error;
        error.line = this->GetLineNumber(definepos,true);
        error.line2 = this->GetLineNumber(end,true);
        error.number = NDEFINE;
        error.description = "#ifndef/#define does not match expression";
        error.description = ifndef+" v.s. "+toMatch;
        m_ErrorList.push_back(error);
        return false;
        }
      else
        {
        return true;
        }
      }

    pos = m_BufferNoComment.find("#ifndef",pos+1);

    if(pos == -1)
      {
      notDefined = true;
      }
    /*else
      {
      for(int i=0;i<pos;i++)
        {
        if((m_BufferNoComment[i] != ' ')
          && (m_BufferNoComment[i] != '\r')
          && (m_BufferNoComment[i] != '\n')
          )
          {
          std::cout << "Not defined!" << std::endl;
          notDefined = true;
          }
        }
      }*/

    } // end looking for entire file


    Error error;
    error.line = this->GetLineNumber(definepos,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#ifndef/#define does not match";
    m_ErrorList.push_back(error);

  return !hasError;
}

} // end namespace kws
