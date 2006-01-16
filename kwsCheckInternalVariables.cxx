/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckInternalVariables.cxx
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



/** Check if the internal variables of the class are correct */
bool Parser::CheckInternalVariables(const char* regEx)
{
  m_TestsDone[IVAR_PUBLIC] = true;
  m_TestsDescription[IVAR_PUBLIC] = "ivars should be in the protected or private section";

  m_TestsDone[IVAR_REGEX] = true;
  m_TestsDescription[IVAR_REGEX] = "ivars should match regular expression: ";
  m_TestsDescription[IVAR_REGEX] += regEx;

  // First we need to find the parameters
  // float myParam;
  bool hasError = false;

  itksys::RegularExpression regex(regEx);

  // First we check in the public area
  long int publicFirst;
  long int publicLast;
  this->FindPublicArea(publicFirst,publicLast);

  long int pos = publicFirst;
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,publicLast,pos);
    if(var == "")
      {
      continue;
      }

    if(var.length() > 0)
      {
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = IVAR_PUBLIC;
      error.description = "Encapsulcation not preserved";
      m_ErrorList.push_back(error);
      hasError = true;
      
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
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,protectedLast,pos);

    if(var == "")
      {
      continue;
      }

    if(var.length() > 0)
      {
      if(!regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = IVAR_REGEX;
        error.description = "Internal variable doesn't match regular expression";
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
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,privateLast,pos);

    if(var == "")
      {
      continue;
      }

    if(var.length() > 0)
      {
      if(!regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = IVAR_REGEX;
        error.description = "Internal variable doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }

  return !hasError;
}

} // end namespace kws
