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
bool Parser::CheckLineLength(unsigned long max,bool checkHeader, bool doErrorCheck)
{
  m_TestsDone[LINE_LENGTH] = true;
  constexpr size_t length = 255;
  char* val = new char[length];
  snprintf(val,length,"Line Length = %ld max chars",max);
  m_TestsDescription[LINE_LENGTH] = val;
  delete [] val;

  m_Positions.clear();
  bool hasError = false;

  size_t fileSize = 0;

  // If we do not want to check the header
  if(!checkHeader)
    {
    if (!m_HeaderFilename.empty()) {
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
    } else {
      // we look at the first '*/' in the file which indicated the end of the current header
      // This assume that there is an header at some point
      size_t endHeader = m_Buffer.find("*/",0);
      if(endHeader != std::string::npos)
        {
        fileSize = endHeader;
        }
    }
    }

  size_t cc;
  const char* inch = m_Buffer.c_str();
  size_t inStrSize = m_Buffer.size();
  long int line_start = 0;
  long int line_end = 0;
  long int line_count = 1;
  m_Positions.push_back(0);
  for ( cc = 0; cc < inStrSize; ++ cc )
    {
    if ( *inch == '\n' )
      {
      m_Positions.push_back(cc);
      line_end = static_cast<long int>(cc);
      long int line_length = line_end - line_start-1;
      if(doErrorCheck && (line_length > (long int)max && cc>fileSize))
        {
        Error error;
        error.line = line_count;
        error.line2 = error.line;
        error.number = LINE_LENGTH;
        error.description = "Line length exceed ";
        constexpr size_t length = 21;
        char localval[length];
        snprintf(localval,length,"%ld",line_length);
        error.description += localval;
        error.description += " (max=";
        snprintf(localval,length,"%ld",max);
        error.description += localval;
        error.description += ")";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      line_start = static_cast<long int>(cc) + 1;
      line_count ++;
      }
    inch ++;
    }
  m_Positions.push_back(cc);

  return !hasError;
}

} // end namespace kws
