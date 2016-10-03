/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckParenthesis.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include <string.h> // strlen

namespace kws {

/** Check the number of spaces after ( and before ) */
bool Parser::CheckParenthesis( unsigned int space )
{
  m_TestsDone[PARENTHESIS] = true;
  m_TestsDescription[PARENTHESIS] =
    "Number of spaces after ( and before ) should be = ";
  char* val = new char[200];
  sprintf(val,"%d", space);
  m_TestsDescription[PARENTHESIS] += val;
  delete [] val;

  bool hasErrors = false;

  long int prevOperatorPos = 0;
  size_t operatorPos = m_BufferNoComment.find("(", prevOperatorPos);
  while(operatorPos != std::string::npos) 
    {
    // Check number of space after
    unsigned int aft = 0;
    long int i = static_cast<long int>( operatorPos+1 );
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
      aft = space;
      }

    if( aft != space &&
      !( i < (long int)m_BufferNoComment.size() && aft == 0 &&
        ( m_BufferNoComment[i] == ')' ||
          m_BufferNoComment[i] == '(' ) ) &&
      !this->IsBetweenChars( '(', ')', operatorPos, false ) &&
      !this->IsBetweenQuote(operatorPos,false) )
      {
      Error error;
      error.line = this->GetLineNumber(operatorPos,true);
      error.line2 = error.line;
      error.number = PARENTHESIS;
      error.description = "Spaces after ( = ";
      char* errorVal = new char[200];
      sprintf(errorVal,"%d",aft);
      error.description += errorVal;
      error.description += " v.s ";
      sprintf(errorVal,"%d",space);
      error.description += errorVal;
      m_ErrorList.push_back(error);
      delete [] errorVal;
      hasErrors = true;
      }

    prevOperatorPos = static_cast<long int>(operatorPos)+1;
    if( prevOperatorPos < (long int)( m_BufferNoComment.size() ) - 1 )
      {
      operatorPos = m_BufferNoComment.find("(", prevOperatorPos);  
      }
    else
      {
      operatorPos = std::string::npos;
      }
    }

  prevOperatorPos = 0;
  operatorPos = m_BufferNoComment.find(")", prevOperatorPos);
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

    if( bfr != space &&
      !( i > 0 && bfr == 0 && 
        ( m_BufferNoComment[i-1] == '(' ||
          m_BufferNoComment[i-1] == ')' ) ) &&
      !this->IsBetweenChars( '(', ')', operatorPos, false ) &&
      !this->IsBetweenQuote(operatorPos,false) )
      {
      Error error;
      error.line = this->GetLineNumber(operatorPos,true);
      error.line2 = error.line;
      error.number = PARENTHESIS;
      error.description = "Spaces before ) = ";
      char* errorVal = new char[200];
      sprintf(errorVal,"%d",bfr);
      error.description += errorVal;
      error.description += " v.s ";
      sprintf(errorVal,"%d",space);
      error.description += errorVal;
      m_ErrorList.push_back(error);
      delete [] errorVal;
      hasErrors = true;
      }

    prevOperatorPos = static_cast<long int>(operatorPos)+1;
    if( prevOperatorPos < (long int)( m_BufferNoComment.size() ) - 1 )
      {
      operatorPos = m_BufferNoComment.find(")", prevOperatorPos);  
      }
    else
      {
      operatorPos = std::string::npos;
      }

    }

  return !hasErrors;
}


} // end namespace kws
