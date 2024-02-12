/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckEmptyLines.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
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
  constexpr size_t length = 255;
  char* val = new char[length];
  snprintf(val,length,"Empty lines = %ld max lines",max);
  m_TestsDescription[EMPTYLINES] = val;
  delete [] val;

  bool hasError = false;
  auto total = static_cast<unsigned long>(m_Buffer.size());
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
        if(m_Buffer.find_first_not_of("\r\n ",i) == std::string::npos)
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
        constexpr size_t length = 10;
        char* localval = new char[length];
        snprintf(localval,length,"%ld",empty);
        error.description += localval;
        error.description += " (max=";
        delete [] localval;
        localval = new char[length];
        snprintf(localval,length,"%ld",max);
        error.description += localval;
        error.description += ")";
        delete [] localval;
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    j++;
    i += static_cast<unsigned long>(line.length())+1;
    }
  return !hasError;
}

} // end namespace kws
