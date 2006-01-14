/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckComments.cxx
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

/** Check the comments
 * The comment definition should be set before CheckIndent() to get the correct indentation
 * for the comments. */
bool Parser::CheckComments(const char* begin,const char* middle,const char* end)
{
  m_TestsDone[WRONGCOMMENT] = true;
  m_TestsDescription[WRONGCOMMENT] = "The comments are misspelled";
  
  bool hasError = false;
  // Set the ivars for the indent checking
  m_CommentBegin = begin;
  m_CommentMiddle = middle;
  m_CommentEnd = end;

  // We check if we have duplicate code in the comments
  std::vector<PairType>::const_iterator it = m_CommentPositions.begin();
  while(it != m_CommentPositions.end())
    {
    std::string previousWord = "";
    long int i = (*it).first;
    while(i<(*it).second)
      {    
      // we go to the next space
      while(((m_Buffer[i] != ' ') && (m_Buffer[i] != '\r') && (m_Buffer[i] != '\r')) && i<(*it).second)
        {
        i++;
        }
      bool inWord = true;
      bool first = false;
      std::string word = "";
      while(i<(*it).second && inWord)
        {
        if(m_Buffer[i] != ' ' && m_Buffer[i] != '\r' && m_Buffer[i] != '\n')
          {
          word += m_Buffer[i];
          inWord = true;
          first = true;
          }
        else // we have a space
          {
          if(first)
            {
            inWord = false;
            i--;
            }
          }
        i++;
        }

      if(word.size()>0)
        {
        if(word != previousWord)
          {
          previousWord = word;
          }
        else if(previousWord.size() > 1 && 
             previousWord[0] != '/'
             ) // avoid single char and comment
          {
          Error error;
          error.line = this->GetLineNumber(i,false);
          error.line2 = error.line;
          error.number = WRONGCOMMENT;
          error.description = "Duplicate word";
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }
    it++;
    } // end buffer loop    

  return !hasError;
}

} // end namespace kws
