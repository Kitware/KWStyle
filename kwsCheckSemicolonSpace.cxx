/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckSemicolonSpace.cxx
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


/** Check the number of space between the end of the declaration
 *  and the semicolon */
bool Parser::CheckSemicolonSpace(unsigned long max)
{
  m_TestsDone[SEMICOLON_SPACE] = true;
  char* val = new char[255];
  sprintf(val,"Semicolons = %d max spaces",max);
  m_TestsDescription[SEMICOLON_SPACE] = val;
  delete [] val;

  bool hasError = false;
  long int posSemicolon = m_BufferNoComment.find(";",0);
  while(posSemicolon != -1)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    unsigned long space = 0;
    while(i>=0)
      {
      if(m_BufferNoComment[i] == ' ')
        {
        space++;
        if(space > max)
          {
          Error error;
          error.line = this->GetLineNumber(posSemicolon,true);
          error.line2 = error.line;
          error.number = SEMICOLON_SPACE;
          error.description = "Number of spaces before semicolon exceed: ";
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
    posSemicolon = m_BufferNoComment.find(";",posSemicolon+1);
    }

  // We also check if we have two or more semicolons following each other
  posSemicolon = m_BufferNoComment.find(";",0);
  while(posSemicolon != -1)
    {
    // We look forward
    unsigned long i=posSemicolon+1;
    while(i<m_BufferNoComment.size())
      {
      if(m_BufferNoComment[i] == ' ' || m_BufferNoComment[i] == '\r' || m_BufferNoComment[i] == '\n')
        {
        // this is normal
        }
      else if(m_BufferNoComment[i] == ';')
        {
        Error error;
        error.line = this->GetLineNumber(i,true);
        error.line2 = error.line;
        error.number = SEMICOLON_SPACE;
        error.description = "Too many semicolons: ";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      else
        {
        break;
        }
      i++;
      }
    posSemicolon = m_BufferNoComment.find(";",i+1);
    }

  return !hasError;
}

} // end namespace kws
