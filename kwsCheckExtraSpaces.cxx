/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckExtraSpaces.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of space between the end of the declaration
 *  and the end of the file */
bool Parser::CheckExtraSpaces(unsigned long max)
{
  m_TestsDone[SPACES] = true;
  char* val = new char[255];
  sprintf(val,"Spaces at the end of line = %d max spaces",max);
  m_TestsDescription[SPACES] = val;
  delete [] val;

  bool hasError = false;
  long int posEndOfLine = m_Buffer.find("\r\n",0);
  while(posEndOfLine != -1)
    {
    // We try to find the word before that
    unsigned long i=posEndOfLine-1;
    unsigned long space = 0;
    while(i>=0)
      {
      if(m_Buffer[i] == ' ')
        {
        space++;
        if(space > max)
          {
          Error error;
          error.line = this->GetLineNumber(posEndOfLine);
          error.line2 = error.line;
          error.number = SEMICOLON_SPACE;
          error.description = "Number of spaces before end of line exceed: ";
          char* val = new char[10];
          sprintf(val,"%d",space);
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
      else
        {
        break;
        }
      i--;
      }
    posEndOfLine = m_Buffer.find("\r\n",posEndOfLine+1);
    }

  return !hasError;
}

} // end namespace kws
