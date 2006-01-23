/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckOperator.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check the number of spaces before and after the operator */
bool Parser::CheckOperator(unsigned int before, unsigned int after)
{
  m_TestsDone[OPERATOR] = true;
  m_TestsDescription[OPERATOR] = "Number of spaces for the operators shoud be: before=";
  char* val = new char[10];
  sprintf(val,"%d",before);
  m_TestsDescription[OPERATOR] += val;  
  sprintf(val,", after=%d",after);
  m_TestsDescription[OPERATOR] += val;
  delete [] val;

  bool hasErrors = false;

  if(!this->FindOperator("==",before,after))
    {
    hasErrors = true;
    }
  
  if(!this->FindOperator("!=",before,after))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("-=",before,after))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("/=",before,after))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("+=",before,after))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("*=",before,after))
    {
    hasErrors = true;
    }

  // We check the = operator but we want to make sure that it is not part
  // of the previous defined operator
  bool doNotCheck = false;
  long int operatorPos = m_BufferNoComment.find("=",0);
  if( (operatorPos == m_BufferNoComment.find("!=",0)+1)
    || (operatorPos == m_BufferNoComment.find("*=",0)+1)
    || (operatorPos == m_BufferNoComment.find("-=",0)+1)
    || (operatorPos == m_BufferNoComment.find("+=",0)+1)
    || (operatorPos == m_BufferNoComment.find("==",0)+1)
    || (operatorPos == m_BufferNoComment.find("operator",0)+9)
    )
    {
    doNotCheck = true;
    }

  while(operatorPos != -1 ) 
    {
    if(!doNotCheck)
      {
      // Check number of space before
      long int bef = 0;
      long int i = operatorPos-1;
      while((i>0) && (m_BufferNoComment[i] == ' '))
        {
        bef++;
        i--;
        }

      // Check number of space before
      long int aft = 0;
      i = operatorPos+1;
      while((i<(long int)m_BufferNoComment.size()) && (m_BufferNoComment[i] == ' '))
        {
        aft++;
        i++;
        }
      
      if(bef != before || aft != after)
        {
        Error error;
        error.line = this->GetLineNumber(operatorPos,true);
        error.line2 = error.line;
        error.number = OPERATOR;
        error.description = "Spaces around operator are wrong";
        //m_ErrorList.push_back(error);
        hasErrors = true;
        }
      }

    doNotCheck = false;
    long int tmpoperatorPos = m_BufferNoComment.find("=",operatorPos+1);
    if( (tmpoperatorPos == m_BufferNoComment.find("!=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("*=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("-=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("+=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("==",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("operator",operatorPos+1)+9)
    )
      {
      doNotCheck = true;
      }

    operatorPos = tmpoperatorPos;
      
    }
  

  return !hasErrors;
}

/** Check the operator */
bool Parser::FindOperator(const char* op,unsigned int before, unsigned int after)
{
  bool hasErrors = false;
  long int pos = 0;
  long int operatorPos = m_BufferNoComment.find(op,pos);
  while(operatorPos != -1 ) 
    {
    // Check number of space before
    long int bef = 0;
    long int i = operatorPos-1;
    while((i>0) && (m_BufferNoComment[i] == ' '))
      {
      bef++;
      i--;
      }

    // Check number of space before
    long int aft = 0;
    i = operatorPos+strlen(op);
    while((i<(long int)m_BufferNoComment.size()) && (m_BufferNoComment[i] == ' '))
      {
      aft++;
      i++;
      }
    
    if(bef != before || aft != after)
      {
      // we make sure that the keyword operator is not defined right before
      if(operatorPos != m_BufferNoComment.find("operator",pos)+9)
        {
        Error error;
        error.line = this->GetLineNumber(operatorPos,true);
        error.line2 = error.line;
        error.number = OPERATOR;
        error.description = "Spaces around operator are wrong";
        m_ErrorList.push_back(error);
        hasErrors = true;
        }
      }
    pos = operatorPos+1;
    operatorPos = m_BufferNoComment.find(op,operatorPos+1);  
    }
  return !hasErrors;
}


} // end namespace kws
