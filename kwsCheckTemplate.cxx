/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckTemplate.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check if the template definition of the class follows
 *  a particular regular expression */
bool Parser::CheckTemplate(const char* regEx)
{
  m_TestsDone[TEMPLATE] = true;
  m_TestsDescription[TEMPLATE] = "Template should match the regex: ";
  m_TestsDescription[TEMPLATE] += regEx;

  kwssys::RegularExpression regex(regEx);

  bool hasErrors = false;
  // We check all the templates in the file
  // Maybe we should separate the main class from the templated function
  // at some point.

  auto templatePos =
      static_cast<long int>(m_BufferNoComment.find("template", 0));
  while(templatePos != -1 ) 
    {
    bool valid = true;

    if(m_BufferNoComment[templatePos-1]!=' ' 
      && m_BufferNoComment[templatePos-1]!='\n')
      {
      valid = false;
      }
    else if(m_BufferNoComment[templatePos+8]!=' ' 
         && m_BufferNoComment[templatePos+8]!='\n')
      {
      valid = false;
      }

    // Definition is template <whatever name,whatever name2 = test, ...>
      auto inf =
          static_cast<long int>(m_BufferNoComment.find("<", templatePos));
      auto sup = static_cast<long int>(m_BufferNoComment.find(">", inf));

      if (inf == -1 || sup == -1) {
        // std::cout << "CheckTemplate(): There is a problem parsing the file"
        // << std::endl;
        valid = false;
      }
    else
      {
      for(long int p=templatePos+8;p<inf;p++)
        {
        if(m_BufferNoComment[p]!=' ' && m_BufferNoComment[p]!='\n'
           && m_BufferNoComment[p]!='\r')
          {
          valid = false;
          break;
          }
        }
      }



    if(!valid)
      {
      templatePos = static_cast<long int>(m_BufferNoComment.find("template",templatePos+1));
      continue;
      }

    long int i = inf+1;
    bool inWord = false;
    bool afterEqual = false;
    int level = 0;
    std::string currentWord = "";

    while(i<=sup)
      {
      if(m_BufferNoComment[i] == '<')
        {
        level++;
        }

      // If we have a space
      if(m_BufferNoComment[i] == ' ')
        {
        // do nothing
        inWord = false;
        }
      else if(m_BufferNoComment[i] == ',' || m_BufferNoComment[i] == '>' || m_BufferNoComment[i] == '=')
        {
        inWord = false;

        if (!currentWord.empty() && !afterEqual && !regex.find(currentWord) &&
            level == 0) {
          Error error;
          error.line = this->GetLineNumber(i,true);
          error.line2 = error.line;
          error.number = TEMPLATE;
          error.description = "Template definition (" + currentWord + ") doesn't match regular expression";
          m_ErrorList.push_back(error);
          hasErrors = true;
        }

        if(m_BufferNoComment[i] == '=')
          {
          afterEqual = true;
          }
        else
          {
          afterEqual = false;
          }
        }
      else
        {
        if(!inWord)
          {
          currentWord = "";
          }
        currentWord += m_BufferNoComment[i];
        inWord = true;
        }

       if(m_BufferNoComment[i] == '>')
        {
        level--;
        }
      i++;
      }

    templatePos = static_cast<long int>(m_BufferNoComment.find("template",templatePos+1));  
    }

  return !hasErrors;
}

} // end namespace kws
