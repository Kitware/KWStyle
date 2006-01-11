/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckIfNDefDefine.cxx
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


/** Check if the #ifndef/#define is defined correctly for the class 
 *  match can contain <NameOfClass> and <Extension> */
bool Parser::CheckIfNDefDefine(const char* match)
{
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

  // Find the first word in the file
  long int pos = m_BufferNoComment.find("#ifndef",0);
  if(pos == -1)
    {
    notDefined = true;
    }
  else
    {
    for(int i=0;i<pos;i++)
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
  pos = m_BufferNoComment.find("#define",end);
  
  if(pos == -1)
    {
    Error error;
    error.line = this->GetLineNumber(end,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#define not defined";
    m_ErrorList.push_back(error);
    return false;
    }
  
  pos += 7; 
  while(m_BufferNoComment[pos] == ' ')
    {
    pos++;
    }
  begin = pos;
  while((m_BufferNoComment[pos] != ' ') 
    &&(m_BufferNoComment[pos] != '\r')
    &&(m_BufferNoComment[pos] != '\n')
    )
    {
    pos++;
    }
  end = pos;
  std::string define = m_BufferNoComment.substr(begin,end-begin);

  if(ifndef != define)
    {
    Error error;
    error.line = this->GetLineNumber(end,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#define does not match #ifndef";
    m_ErrorList.push_back(error);
    return false;
    }

  if(m_Filename == "")
    {
    std::cout << "CheckIfNDefDefine() : m_Filename shoud be set" << std::endl;
    return false;
    }

  long int point = m_Filename.find_last_of(".");
  long int slash = m_Filename.find_last_of("/");

  if(slash == -1)
    {
    slash = 0;
    }

  std::string nameofclass = m_Filename.substr(slash+1,point-slash-1);
  std::string extension = m_Filename.substr(point+1,m_Filename.size()-point-1);

  // construct the string
  std::string toMatch = match;
  pos = toMatch.find("<NameOfClass>");
  if(pos != -1)
    {
    toMatch.replace(pos,13,nameofclass);
    }
  pos = toMatch.find("<Extension>");
  if(pos != -1)
    {
    toMatch.replace(pos,11,extension);
    }


  if(ifndef != toMatch)
    {   
    Error error;
    error.line = this->GetLineNumber(definepos,true);
    error.line2 = this->GetLineNumber(end,true);
    error.number = NDEFINE;
    error.description = "#ifndef/#define does not match expression";
    m_ErrorList.push_back(error);
    return false;
    }

  return !hasError;
}

} // end namespace kws
