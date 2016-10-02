/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckComma.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include <string.h> // strlen

namespace kws {

/** Check the number of spaces after the if keyword */
bool Parser::CheckComma( unsigned int before, unsigned int after )
{
  m_TestsDone[COMMA] = true;
  m_TestsDescription[COMMA] =
    "Number of spaces for , should be:";
  char* val = new char[10];
  sprintf(val," before = %d",before);
  m_TestsDescription[COMMA] += val;
  sprintf(val," : after = %d",after);
  m_TestsDescription[COMMA] += val;
  delete [] val;

  bool hasErrors = false;
  
  long int prevOperatorPos = 0;
  size_t operatorPos = m_BufferNoComment.find(",", prevOperatorPos);
  while(operatorPos != std::string::npos) 
    {
    // Check number of space before
    unsigned int bfr = 0;
    long int i = static_cast<long int>( operatorPos );
    while( i>0 && m_BufferNoComment[i-1] == ' ' )
      {
      ++bfr;
      --i;
      }

    // Check number of space after
    unsigned int aft = 0;
    i = static_cast<long int>( operatorPos+1 );
    while( i<(long int)m_BufferNoComment.size()
           && m_BufferNoComment[i] == ' ' )
      {
      aft++;
      i++;
      }
    if( i<(long int)m_BufferNoComment.size()
        && ( m_BufferNoComment[i] == '\n' ||
             m_BufferNoComment[i] == '\r' ) )
      {
      aft = after;
      }

    if( bfr != before || aft != after )
      {
      if( !this->IsBetweenQuote(operatorPos,false) )
        {
        Error error;
        error.line = this->GetLineNumber(operatorPos,true);
        error.line2 = error.line;
        error.number = COMMA;
        error.description = "Spaces before , = ";
        char* val = new char[10];
        sprintf(val,"%d",bfr);
        error.description += val;
        error.description += " v.s ";
        sprintf(val,"%d",before);
        error.description += val;
        error.description += " : Spaces after , = ";
        sprintf(val,"%d",aft);
        error.description += val;
        error.description += " v.s ";
        sprintf(val,"%d",after);
        error.description += val;
        m_ErrorList.push_back(error);
        delete [] val;
        hasErrors = true;
        }
      }

    prevOperatorPos = static_cast<long int>(operatorPos)+1;
    if( prevOperatorPos < m_BufferNoComment.size()-1 )
      {
      operatorPos = m_BufferNoComment.find(",", prevOperatorPos);  
      }
    else
      {
      operatorPos = std::string::npos;
      }

    }

  return !hasErrors;
}


} // end namespace kws
