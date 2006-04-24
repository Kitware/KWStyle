/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckLineLength.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of character per line */
bool Parser::CheckLineLength(unsigned long max,bool checkHeader)
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

  unsigned long fileSize = 0;
  
  // If we do not want to check the header
  if(!checkHeader)
    {
    if(m_HeaderFilename.size()>0)
      {
      std::ifstream file;
      file.open(m_HeaderFilename.c_str(), std::ios::binary | std::ios::in);
      if(!file.is_open())
        {
        std::cout << "CheckLineLength() - Cannot open file: " 
                  << m_HeaderFilename << std::endl;
        }
      else
        {
        file.seekg(0,std::ios::end);
        fileSize = file.tellg();
        file.close();
        }
      }
    else
      {
      // we look at the first '*/' in the file which indicated the end of the current header
      // This assume that there is an header at some point  
      long int endHeader = m_Buffer.find("*/",0);
      if(endHeader>0)
        {
        fileSize = endHeader;
        }
      }
    }

  size_t cc;
  const char* inch = m_Buffer.c_str();
  size_t inStrSize = m_Buffer.size();
  size_t line_start = 0;
  size_t line_end = 0;
  size_t line_count = 1;
  m_Positions.push_back(0);
  for ( cc = 0; cc < inStrSize; ++ cc )
    {
    if ( *inch == '\n' )
      {
      m_Positions.push_back(cc);
      line_end = cc;
      size_t line_length = line_end - line_start;
      if(line_length > max && cc>fileSize)
        {
        Error error;
        error.line = line_count;
        error.line2 = error.line;
        error.number = LINE_LENGTH;
        error.description = "Line length exceed ";
        char* val = new char[10];
        sprintf(val,"%d",line_length);
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
      line_start = cc + 1;
      line_count ++;
      }
    inch ++;
    }
  m_Positions.push_back(cc);

  return !hasError;
}

} // end namespace kws
