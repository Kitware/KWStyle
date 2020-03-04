/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckNameOfClass.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check if the name of the class 
 *  The class name is the one that is not between anything and does not have ;
 *  before the } */
bool Parser::CheckNameOfClass(const char* name,const char* prefix)
{
  m_TestsDone[NAMEOFCLASS] = true;
  m_TestsDescription[NAMEOFCLASS] = "The name of the class should match ";
  if(prefix)
    {
    m_TestsDescription[NAMEOFCLASS] += prefix;
    }
  m_TestsDescription[NAMEOFCLASS] += name;
  
  bool gotMatch = false;
  bool gotAtLeastOne = false;

  long int classpos = static_cast<long int>(this->GetClassPosition(0));
  std::string nameOfClass = "";
  while(classpos!=-1)
    {
    nameOfClass = this->FindPreviousWord(classpos);

    if (m_Filename.empty()) {
      std::cout << "CheckNameOfClass() : m_Filename should be set" << std::endl;
      return false;
    }

    auto point = static_cast<long int>(m_Filename.find_last_of("."));
    auto slash = static_cast<long int>(m_Filename.find_last_of("/"));

    if(slash == -1)
      {
      slash = 0;
      }

    std::string nameofclass = m_Filename.substr(slash+1,point-slash-1);
    std::string extension = m_Filename.substr(point+1,m_Filename.size()-point-1);

    // construct the string
    std::string toMatch = name;
    auto p = static_cast<long int>(toMatch.find("[NameOfClass]"));
    if(p != -1)
      {
      toMatch.replace(p,13,nameofclass);
      }
    p = static_cast<long int>(toMatch.find("[Extension]"));
    if(p != -1)
      {
      toMatch.replace(p,11,extension);
      }
        
    if(prefix)
      {
      nameOfClass = prefix+nameOfClass;
      }

    gotAtLeastOne = true;
    if(nameOfClass == toMatch)
      {
      gotMatch = true;
      break; 
      }
    classpos = static_cast<long int>(this->GetClassPosition(classpos+1));
    }

  if(!gotMatch && gotAtLeastOne && (classpos!=-1))
    {
    Error error;
    error.line = this->GetLineNumber(classpos,true);
    error.line2 = error.line;
    error.number = NAMEOFCLASS;
    error.description = "classname is not defined correctly";
    m_ErrorList.push_back(error);
    return false;
    }

  return true;
}


/** Returns the position of a class within the file.
 *  This function checks that we are in the class definition and not somewhere else 
 *  The return position is a position right after the name of the class (meaning before a : or a { 
 *  Returns std::string::npos if not found
 */
size_t Parser::GetClassPosition(size_t position,std::string buffer) const
{
  if(position == std::string::npos)
   {
   position = 0;
   }

   if (buffer.empty()) {
     buffer = m_BufferNoComment;
   }

  size_t pos = buffer.find("class",position);

  std::string nameOfClass = "";
  while(pos!=std::string::npos)
    {
    if(!this->IsBetweenCharsFast('<','>',pos,false,buffer))
      {
      bool valid = true;      
      // We check that the word class is alone
      if(pos>1)
        {
        if(buffer[pos-1] != ' ' && buffer[pos-1] != '/' && buffer[pos-1] != '\n')
          {
          valid = false;
          }
        }
      if(pos<buffer.size()-2)
        {
        if(buffer[pos+5] != ' ' && buffer[pos+5] != '/' && buffer[pos+5] != '\r')
          {
          valid = false;
          } 
        }
      
      size_t i = pos+4;
      // We should get a { before a ;
      size_t brac = buffer.find('{',pos);
      size_t sem = buffer.find(';',pos);

      if(sem<=brac)
        {
        valid = false;
        }
      else
        {
        while((buffer[i] != '{')
          && (i<buffer.size())
          )
          {
          if(buffer[i] == ';')
            {
            valid = false;
            break;
            }
          else if(buffer[i] == ':')
            {
            break;
            }
          i++;
          }
        }
      
      if(valid)
        {
        return i;
        }
      }
    pos = buffer.find("class",pos+1);
    }

  return std::string::npos;
}

} // end namespace kws
