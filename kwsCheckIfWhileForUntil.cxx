/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckIfWhileForUntil.cxx

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
bool Parser::CheckIfWhileForUntil(unsigned int after)
{
  m_TestsDone[IFWHILEFORUNTIL] = true;
  m_TestsDescription[IFWHILEFORUNTIL] =
    "Number of spaces after If/While/For should be";
  char* val = new char[10];
  sprintf(val," = %d",after);
  m_TestsDescription[IFWHILEFORUNTIL] += val;
  delete [] val;

  bool hasErrors = false;
  
  if(!this->FindIfWhileForUntil("if",after))
    {
    hasErrors = true;
    }
  if(!this->FindIfWhileForUntil("while",after))
    {
    hasErrors = true;
    }
  if(!this->FindIfWhileForUntil("for",after))
    {
    hasErrors = true;
    }
  if(!this->FindIfWhileForUntil("until",after))
    {
    hasErrors = true;
    }

  return !hasErrors;
}

/** Check the operator */
bool Parser::FindIfWhileForUntil(const char* op,
                          unsigned int after)
{
  bool hasErrors = false;

  long int prevOperatorPos = 0;
  size_t operatorPos = m_BufferNoComment.find(op,prevOperatorPos);
  while(operatorPos != std::string::npos) 
    {
    bool checkError = true;
    if( operatorPos > 0 && !isspace( m_BufferNoComment[operatorPos-1] ) )
      {
      checkError = false;
      }
    if( operatorPos+strlen(op) < m_BufferNoComment.size() &&
      !isspace( m_BufferNoComment[operatorPos+strlen(op)] ) &&
      m_BufferNoComment[operatorPos+strlen(op)] != '(' )
      {
      checkError = false;
      }

    if( checkError )
      {
      // Check number of space after
      unsigned int aft = 0;
      long int i = static_cast<long int>( operatorPos+strlen(op) );
      while( (i<(long int)m_BufferNoComment.size())
             && m_BufferNoComment[i] == ' ' )
        {
        aft++;
        i++;
        }
      if( i<(long int)m_BufferNoComment.size() &&
        ( m_BufferNoComment[i] == '\n' || m_BufferNoComment[i] == '\r' ) )
        {
        aft = after + 1;
        }

      if( aft != after )
        {
        if( !this->IsBetweenQuote(operatorPos,false) )
          {
          Error error;
          error.line = this->GetLineNumber(operatorPos,true);
          error.line2 = error.line;
          error.number = IFWHILEFORUNTIL;
          error.description = "Spaces after ";
          error.description += op;
          error.description += " = ";
          char* val = new char[10];
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
      }
    prevOperatorPos = static_cast<long int>(operatorPos)+1;
    if( prevOperatorPos < m_BufferNoComment.size()-1 )
      {
      operatorPos = m_BufferNoComment.find(op,prevOperatorPos);  
      }
    else
      {
      operatorPos = std::string::npos;
      }
    }
  return !hasErrors;
}


} // end namespace kws
