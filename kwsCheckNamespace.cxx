/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckNamespace.cxx
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


/** Find the name space 
 *  using namespace is prohibited for this parser 
 *  We look if the closing bracket is at the end of the file, if yes we accept otherwise we reject*/
bool Parser::CheckNamespace(const char* name)
{
  m_TestsDone[NAMESPACE] = true;
  char* val = new char[255];
  sprintf(val,"The first namespace of the file should match %s",name);
  m_TestsDescription[NAMESPACE] = val;
  delete [] val;

  std::string nameSpace = "";

  long int pos = m_BufferNoComment.find("namespace",0);

  if(pos == -1)
    {
    Error error;
    error.line = this->GetLineNumber(0,true);
    error.line2 = error.line;
    error.number = NAMESPACE;
    error.description = "namespace not defined";
    m_ErrorList.push_back(error);
    return false;
    }

  
  // check that the word is not using namespace
  bool ok = true;
  if(this->FindPreviousWord(pos) == "using")
    {
    ok = false;
    }

  if(ok)
    {
    std::string names = name;
    // extract the namespace
    std::string nspace = this->FindNextWord(pos);
    if(nspace != names)
      {
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = NAMESPACE;
      error.description = "namespace is wrong";
      m_ErrorList.push_back(error);
      return false;
      }
    }

  /*
  // Now search if we have other namespaces
  while(pos!= -1 && ok)
    {
    pos = buffer.find("namespace",pos+1);
    // check that the word is not using namespace
    use = buffer.find("using",pos-20);

    ok = true;

    if((use!=-1 && pos-use<20) || (pos == -1))
      {
      ok = false;
      }

    if(ok)
      {
      // extract the namespace
      long int bracket = buffer.find("{",pos);
      long int closing = FindClosingBracket(buffer,bracket);

      // Find the number of } after closing
      int num = 0;
      int close = closing+1;
      while(buffer.find("}",close) != -1)
        {
        num++;
        close=buffer.find("}",close);
        close++;
        }

      if(bracket != -1 && bracket-pos-10<10 && num==nNameSpace)
        {
        std::string name = buffer.substr(pos+10,bracket-pos-10);
        RemoveCtrlN(name);
        RemoveChar(name,' ');
        RemoveCtrlN(name);
        nameSpace += "::";
        nameSpace += name;
        nNameSpace++;
        }
      }
    }
    */

  return true;
}

} // end namespace kws
