/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckExtraSpaces.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of space between the end of the declaration
 *  and the end of the file */
bool Parser::CheckExtraSpaces(unsigned long max,bool checkEmptyLines)
{
  m_TestsDone[SPACES] = true;
  char* val = new char[255];
  sprintf(val,"Spaces at the end of line = %ld max spaces",max);
  m_TestsDescription[SPACES] = val;
  delete [] val;

  bool hasError = false;
  long int posEndOfLine = m_Buffer.find("\r\n",0);
  while(posEndOfLine != -1)
    {
    bool checking = true;
    if(!checkEmptyLines)
      {
      // Check if the line is empty
      long pos = posEndOfLine-1;
      bool empty = true;
      while(pos>0 && m_Buffer[pos]!='\n')
        {
        if(m_Buffer[pos] != ' ')
          {
          empty = false;
          break;
          }
        pos--;
        }

      if(empty)
        {
        checking = false;
        }
      }

     if(checking)
     {
      // We try to find the word before that
      long i=posEndOfLine-1;
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
            error.number = SPACES;
            error.description = "Number of spaces before end of line exceed: ";
            char* localval = new char[10];
            sprintf(localval,"%ld",space);
            error.description += localval;
            error.description += " (max=";
            delete [] localval;
            localval = new char[10];
            sprintf(localval,"%ld",max);
            error.description += localval;
            error.description += ")";
            delete [] localval;
            m_ErrorList.push_back(error);
            hasError = true;
            break; // avoid multiple error line
            }
          }
        else
          {
          break;
          }
        i--;
        }
      }
    posEndOfLine = m_Buffer.find("\r\n",posEndOfLine+1);
    }

  return !hasError;
}

} // end namespace kws
