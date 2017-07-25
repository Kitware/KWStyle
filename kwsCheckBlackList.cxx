/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckBlackList.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include <string.h>

namespace kws {


/** Check that there is no word in the class that matches a word in the black list */
bool Parser::CheckBlackList(const char* filename)
{
  m_TestsDone[BLACKLIST] = true;
  m_TestsDescription[BLACKLIST] = "No word should match any words from the file: ";
  m_TestsDescription[BLACKLIST] += filename;

  bool hasError = false;

  // If the black list is not read we read it
  if(strcmp(m_BlackList.c_str(),filename))
    {
    // Read the file
    std::ifstream file;
    file.open(filename, std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open file: " << filename << std::endl;
      return 0;
      }
    file.seekg(0,std::ios::end);
    unsigned long fileSize = file.tellg();
    file.seekg(0,std::ios::beg);

    char* buf = new char[fileSize+1];
    file.read(buf,fileSize);
    buf[fileSize] = 0; 
    m_BlackListBuffer = buf;
    m_BlackListBuffer.resize(fileSize);
    delete [] buf;
    m_BlackList = filename;
    file.close();
    }

  // Go through the list of words
  size_t start = 0;
  size_t space = m_BlackListBuffer.find(" ",start);
  size_t eol = m_BlackListBuffer.find("\n",start);

  size_t end = space;
  if(eol != std::string::npos && eol < space)
    {
    if(eol == 0)
      {
      end = std::string::npos;
      }
    else
      {
      end = eol-1;
      }
    }

  if(end == std::string::npos && eol != std::string::npos)
    {
    if(eol == 0)
      {
      end = std::string::npos;
      }
    else
      {
      end = eol-1;
      }
    }

  while(end != std::string::npos && end>start)
    {
    std::string blackword = m_BlackListBuffer.substr(start,end-start);
    start = end+1;
    
    space = m_BlackListBuffer.find(" ",start);
    eol = m_BlackListBuffer.find("\n",start);

    end = space;
    if(eol != std::string::npos && eol < space)
      {
      if(eol == 0)
        {
        end = std::string::npos;
        }
      else
        {
        end = eol-1;
        }
      }
    if(end == std::string::npos && eol != std::string::npos)
      {
      if(eol == 0)
        {
        end = std::string::npos;
        }
      else
        {
        end = eol-1;
        }
      }

    if(blackword.length() <= 1)
      {
      continue;
      }

    size_t pos = m_BufferNoComment.find(blackword,0);
    while(pos != std::string::npos)
      {
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = BLACKLIST;
      error.description = "The word is in the black list";
      m_ErrorList.push_back(error);
      hasError = true;
      pos = m_BufferNoComment.find(blackword,pos+1);
      }
    }
  return !hasError;
}

} // end namespace kws
