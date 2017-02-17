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
  char* val = new char[200];
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
             m_BufferNoComment[i] == '\r' ||
             m_BufferNoComment[i] == '\\' ) )
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
        char* errorVal = new char[200];
        sprintf(errorVal,"%d",bfr);
        error.description += errorVal;
        error.description += " v.s ";
        sprintf(errorVal,"%d",before);
        error.description += errorVal;
        error.description += " : Spaces after , = ";
        sprintf(errorVal,"%d",aft);
        error.description += errorVal;
        error.description += " v.s ";
        sprintf(errorVal,"%d",after);
        error.description += errorVal;
        m_ErrorList.push_back(error);
        delete [] errorVal;
        hasErrors = true;
        }
      }

    prevOperatorPos = static_cast<long int>(operatorPos)+1;
    if( prevOperatorPos < (long int)( m_BufferNoComment.size() )-1 )
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
