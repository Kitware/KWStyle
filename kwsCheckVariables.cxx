/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckVariables.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the variables implementation of the class are correct */
bool Parser::CheckVariables(const char* regEx)
{
  m_TestsDone[VARS] = true;
  m_TestsDescription[VARS] = "ivars implentation should match regular expression: ";
  m_TestsDescription[VARS] += regEx;

  // First we need to find the parameters
  bool hasError = false;
  kwssys::RegularExpression regex(regEx);

  // We first read the .h if any
  std::string headerfile = kwssys::SystemTools::GetFilenamePath(m_Filename.c_str());
  headerfile += "/";
  headerfile += kwssys::SystemTools::GetFilenameWithoutExtension(m_Filename.c_str());
  headerfile += ".h";

  if(!kwssys::SystemTools::FileExists(headerfile.c_str()))
    {
    return false;
    }

  // We open the file
  std::ifstream file;
  file.open(headerfile.c_str(), std::ios::binary | std::ios::in);
  if(!file.is_open())
    {
    std::cout << "Cannot open file: " << headerfile.c_str() << std::endl;
    return 1;
    }

  file.seekg(0,std::ios::end);
  unsigned long fileSize = file.tellg();
  file.seekg(0,std::ios::beg);

  char* buf = new char[fileSize+1];
  file.read(buf,fileSize);
  buf[fileSize] = 0; 
  std::string buffer(buf);
  buffer.resize(fileSize);
  delete [] buf;
  
  file.close();
  
  this->ConvertBufferToWindowsFileType(buffer);
  buffer = this->RemoveComments(buffer.c_str());
  
  // Construct the list of variables to check
  std::vector<std::string> ivars;
  long int pos = 0;
  while(pos != -1)
    {
    std::string var = this->FindVariable(buffer,pos+1,buffer.size(),pos);
    if(var.size()>0)
      {
      std::string correct = "";
      for(unsigned long i=0;i<var.size();i++)
        {
        if(var[i] != '*')
          {
          correct+=var[i];
          }
        }
      ivars.push_back(correct);
      }
    }

  // Do the checking
  std::vector<std::string>::const_iterator it = ivars.begin();
  while(it != ivars.end())
    {
    long posVar = m_BufferNoComment.find(*it);
    while(posVar != -1)
      {
      // Extract the complete insert of the variable
      unsigned long
      while(




      // Check the regex
      if(!regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = VARS;
        error.description = "variable (" + var + ") doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }

      posVar = m_BufferNoComment.find(*it,posVar+1);
      }
    it++;
    }

  return !hasError;
}

/** Find the first ivar in the source code */
std::string Parser::FindVariable(std::string & buffer, long int start, long int end,long int & pos)
{
  long int posSemicolon = buffer.find(";",start);
  while(posSemicolon != -1 && posSemicolon<end)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    bool inWord = true;
    bool first = false;
    std::string ivar = "";
    while(i>=0 && inWord)
      {
      if(buffer[i] != ' ')
        {
        if((buffer[i] == '}')
          || (buffer[i] == ')')
          || (buffer[i] == ']')
          || (buffer[i] == '\n')
          )
          {
          inWord = false;
          }
        else
          {
          std::string store = ivar;
          ivar = buffer[i];
          ivar += store;
          inWord = true;
          first = true;
          }
        }
      else // we have a space
        {
        if(first)
          {
          inWord = false;
          }
        }
      i--;
      }
    pos = posSemicolon;
    // We extract the complete definition.
    // This means that we look for a '{' or '}' or '{' or ':'
    // but not '::'
    while(i>0)
      {
      if(buffer[i] == ';')
        {
        break;
        }
      else if(buffer[i] == ':')
        {
        if((buffer[i-1] != ':') && (buffer[i+1] != ':'))
          {
          break;
          }
        }
      i--;
      }

    std::string subphrase = "";

    if(i>=0)
      {
      subphrase = buffer.substr(i+1,posSemicolon-i-1);
      }

    if( (subphrase.find("=") == -1)
      && (subphrase.find("(") == -1)
      && (subphrase.find("typedef") == -1)
      && (subphrase.find("}") == -1)
      && (subphrase.find("friend") == -1)
      && (subphrase.find("class") == -1)
      && (subphrase.find("return") == -1)
      && (subphrase.find("\"") == -1)
      && (subphrase.find("<<") == -1)
      )
      {
      // Check that we are not inside a function(){}
      if(!this->IsInFunction(posSemicolon,buffer.c_str())
        && !this->IsInStruct(posSemicolon,buffer.c_str())
        )
        {
        return ivar;
        }
      }

    posSemicolon = buffer.find(";",posSemicolon+1);
    }

  pos = -1;
  return "";
}

} // end namespace kws
