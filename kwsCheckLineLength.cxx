/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckLineLength.cxx
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


/** Check the number of character per line */
bool Parser::CheckLineLength(unsigned long max)
{
  m_TestsDone[LINE_LENGTH] = true;
  char* val = new char[255];
  sprintf(val,"Line Length = %d max chars",max);
  m_TestsDescription[LINE_LENGTH] = val;
  delete [] val;

  m_Positions.clear();
  unsigned long total = m_Buffer.size();
  unsigned long i = 0;
  unsigned long j = 1;
  bool hasError = false;
  while(i<total)
    {
    // extract the line
    std::string line = m_Buffer.substr(i+1,m_Buffer.find("\n",i+1)-i-1);
    m_Positions.push_back(i);
    if(line.length() > max)
      {
      Error error;
      error.line = j;
      error.line2 = error.line;
      error.number = LINE_LENGTH;
      error.description = "Line length exceed ";
      char* val = new char[10];
      sprintf(val,"%d",line.length());
      error.description += val;
      error.description += " (max=";
      delete [] val;
      val = new char[10];
      sprintf(val,"%d",max);
      error.description += val;
      error.description += ")";
      delete [] val;
      m_ErrorList.push_back(error);
      hasError = true;
      }
    j++;
    i += line.length()+1;
    }

  m_Positions.push_back(total-1);
    
  return !hasError;
}

} // end namespace kws
