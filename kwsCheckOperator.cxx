/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckOperator.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include <string.h> // strlen

namespace kws {

/** Check the number of spaces before and after the operator */
bool Parser::CheckOperator(unsigned int before, unsigned int after,
                           unsigned long maxSize,
                           bool doNotCheckInsideParenthesis)
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

  if(!this->FindOperator("==",before,after,maxSize,doNotCheckInsideParenthesis))
    {
    hasErrors = true;
    }
  
  if(!this->FindOperator("!=",before,after,maxSize,doNotCheckInsideParenthesis))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("-=",before,after,maxSize,doNotCheckInsideParenthesis))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("/=",before,after,maxSize,doNotCheckInsideParenthesis))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("+=",before,after,maxSize,doNotCheckInsideParenthesis))
    {
    hasErrors = true;
    }

  if(!this->FindOperator("*=",before,after,maxSize,doNotCheckInsideParenthesis))
    {
    hasErrors = true;
    }

  // We check the = operator but we want to make sure that it is not part
  // of the previous defined operator
  bool doNotCheck = false;
  size_t operatorPos = m_BufferNoComment.find("=",0);

  if( (doNotCheckInsideParenthesis && this->IsBetweenCharsFast('(',')',operatorPos,false)) 
    || (operatorPos == m_BufferNoComment.find("!=",0)+1)
    || (operatorPos == m_BufferNoComment.find("*=",0)+1)
    || (operatorPos == m_BufferNoComment.find("-=",0)+1)
    || (operatorPos == m_BufferNoComment.find("+=",0)+1)
    || (operatorPos == m_BufferNoComment.find("==",0)+1)
    || (operatorPos == m_BufferNoComment.find("operator",0)+9)
    )
    {
    doNotCheck = true;
    }

  while(operatorPos != std::string::npos) 
    {
    if(!doNotCheck)
      {
      // Check number of space before
      unsigned int bef = 0;
      long int i = operatorPos-1;
      while((i>0) && (m_BufferNoComment[i] == ' '))
        {
        bef++;
        i--;
        }

      // Check number of space after
      unsigned int aft = 0;
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
    size_t tmpoperatorPos = m_BufferNoComment.find("=",operatorPos+1);

    if( (doNotCheckInsideParenthesis && this->IsBetweenCharsFast('(',')',tmpoperatorPos,false)) 
    ||  (tmpoperatorPos == m_BufferNoComment.find("!=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("*=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("-=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("+=",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("==",operatorPos+1)+1)
    || (tmpoperatorPos == m_BufferNoComment.find("operator",operatorPos+1)+8)
    )
      {
      doNotCheck = true;
      }

    operatorPos = tmpoperatorPos;
      
    }
  

  return !hasErrors;
}

/** Check the operator */
bool Parser::FindOperator(const char* op,unsigned int before, 
                          unsigned int after,
                          unsigned long maxSize,
                          bool doNotCheckInsideParenthesis
                          )
{
  bool hasErrors = false;
  long int pos = 0;
  size_t operatorPos = m_BufferNoComment.find(op,pos);
  while(operatorPos != std::string::npos) 
    {
    bool showError=true;
    // Check number of space before
    unsigned int bef = 0;
    long int i = operatorPos-1;
    while((i>0) && ((m_BufferNoComment[i] == ' ')
       || (m_BufferNoComment[i] == '\r')
       || (m_BufferNoComment[i] == '\n'))
       )
      {
      if((m_BufferNoComment[i] != '\r')
       && (m_BufferNoComment[i] != '\n'))
        {
        bef++;
        }
      else
        {
        // check if the sum of the two lines is higher than the maximum length
        std::string currentLine = this->GetLine(this->GetLineNumber(i,true)-1);
        std::string previousLine = this->GetLine(this->GetLineNumber(i,true));
        unsigned long sum = currentLine.size()+previousLine.size();
        if(sum>maxSize)
          {
          showError = false;
          break;
          }
        }
      i--;
      }

    // Check number of space after
    unsigned int aft = 0;
    i = operatorPos+strlen(op);
    while((i<(long int)m_BufferNoComment.size())
           && ((m_BufferNoComment[i] == ' ')
           || (m_BufferNoComment[i] == '\r')
           || (m_BufferNoComment[i] == '\n')
           )
           )
      {
      if((m_BufferNoComment[i] == '\r')
        || (m_BufferNoComment[i] == '\n'))
        {
        // check if the sum of the two lines is higher than the maximum length
        std::string currentLine = this->GetLine(this->GetLineNumber(i,true)-1);
        std::string nextLine = this->GetLine(this->GetLineNumber(i,true));
        unsigned long sum = currentLine.size()+nextLine.size();
        if(sum>maxSize)
          {
          showError = false;
          break;
          }
        }
      else
        {
        aft++;
        }
      i++;
      }
   
    if(showError  && (bef != before || aft != after))
      {
      // we make sure that the keyword operator is not defined right before
      if(operatorPos != m_BufferNoComment.find("operator",operatorPos-11)+8
        && (doNotCheckInsideParenthesis && !this->IsBetweenCharsFast('(',')',operatorPos,false)
        && !this->IsBetweenQuote(operatorPos,false)
        ) 
        )
        {
        Error error;
        error.line = this->GetLineNumber(operatorPos,true);
        error.line2 = error.line;
        error.number = OPERATOR;
        error.description = "Spaces around operator are wrong: ";
        error.description += "before = ";
        char* val = new char[10];
        sprintf(val,"%d",bef);
        error.description += val;
        error.description += " v.s ";
        sprintf(val,"%d",before);
        error.description += val;
        error.description += " ,after = ";
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
    pos = operatorPos+1;
    operatorPos = m_BufferNoComment.find(op,operatorPos+1);  
    }
  return !hasErrors;
}


} // end namespace kws
