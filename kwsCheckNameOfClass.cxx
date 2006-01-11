/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckNameOfClass.cxx
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


/** Check if the name of the class 
 *  The class name is the one that is not between anything and does not have ;
 *  before the } */
bool Parser::CheckNameOfClass(const char* name,const char* prefix)
{
  m_TestsDone[NAMEOFCLASS] = true;

  bool gotMatch = false;
  long int pos = m_BufferNoComment.find("class",0);
  long int errorpos = 0;
  std::string nameOfClass = "";
  while(pos!=-1)
    {
    if(!IsBetweenChars('<','>',pos))
      {
      bool valid = true;
      // We should get a { before a ;
      long int i = pos+4;
      while((m_BufferNoComment[i] != '{')
         && (i<(long)m_BufferNoComment.size())
         )
        {
        if(m_BufferNoComment[i] == ';')
          {
          valid = false;
          break;
          }
        else if(m_BufferNoComment[i] == ':')
          {
          break;
          }
        i++;
        }

      if(valid)
        {
        errorpos = i;
        nameOfClass = this->FindPreviousWord(i);

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
        std::string toMatch = name;
        long int p = toMatch.find("<NameOfClass>");
        if(p != -1)
          {
          toMatch.replace(p,13,nameofclass);
          }
        p = toMatch.find("<Extension>");
        if(p != -1)
          {
          toMatch.replace(p,11,extension);
          }

        nameOfClass = prefix+nameOfClass;

        if(nameOfClass == toMatch)
          {
          gotMatch = true;
          break;
          }
        }
      }
    pos = m_BufferNoComment.find("class",pos+1);
    }

  if(!gotMatch)
    {
    Error error;
    error.line = this->GetLineNumber(errorpos,true);
    error.line2 = error.line;
    error.number = NAMEOFCLASS;
    error.description = "classname is not defined correctly";
    m_ErrorList.push_back(error);
    return false;
    }

  return true;
}

} // end namespace kws
