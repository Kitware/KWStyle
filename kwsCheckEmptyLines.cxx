/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckEmptyLines.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of succesive empty lines */
bool Parser::CheckEmptyLines(unsigned long max, bool checkEndOfFile)
{
  m_TestsDone[EMPTYLINES] = true;
  char* val = new char[255];
  sprintf(val,"Empty lines = %d max lines",max);
  m_TestsDescription[EMPTYLINES] = val;
  delete [] val;

  bool hasError = false;
  unsigned long total = m_Buffer.size();
  unsigned long i = 0;
  unsigned long j = 1;
  unsigned long empty = 0;
  while(i<total)
    {
    // extract the line
    std::string line = m_Buffer.substr(i+1,m_Buffer.find("\n",i+1)-i-1);
    
    // if we have \n\r
    if(line.length() <= 1)
      {
      empty++;
      }
    else
      {
      empty = 0;
      }

    if(empty>max)
      {
      bool valid = true;
      // Check if we are at the end of the file
      if(!checkEndOfFile)
        {
        if(m_Buffer.find_first_not_of("\r\n ",i) == -1)
          {
          valid = false;
          }
        }

      if(valid)
        {
        Error error;
        error.line = j;
        error.line2 = error.line;
        error.number = EMPTYLINES;
        error.description = "Empty lines exceed ";
        char* val = new char[10];
        sprintf(val,"%d",empty);
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
      }
    j++;
    i += line.length()+1;
    }
  return !hasError;
}

} // end namespace kws
