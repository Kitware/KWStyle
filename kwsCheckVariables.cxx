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
  m_TestsDescription[VARS] = "ivars implementation should match regular expression: ";
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
    return true;
    }

  file.seekg(0,std::ios::end);
  size_t fileSize = file.tellg();
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
  typedef std::pair<std::string,long int> PairType;
  std::vector<PairType> ivars;
  size_t pos = 0;
  while(pos != std::string::npos)
    {
    std::string var = this->FindVariable(buffer,pos+1,buffer.size(),pos);
    if (!var.empty()) {
      std::string correct = "";
      for(size_t i=0;i<var.size();i++)
        {
        if(var[i] != '*')
          {
          correct+=var[i];
          }
        }
      PairType p;
      p.first = correct;
      p.second = this->GetLineNumber(pos,true)-1;
      ivars.push_back(p);
    }
    }

  // Do the checking
  std::vector<PairType>::const_iterator it = ivars.begin();
  while(it != ivars.end())
    {
    std::string v = (*it).first;
    long int p =(*it).second;
    size_t posVar = m_BufferNoComment.find(v);
    while(posVar != std::string::npos)
      {
      // Extract the complete insert of the variable
      if(!this->IsBetweenQuote(posVar)
        &&(
        m_BufferNoComment[posVar-1]=='.'
        || m_BufferNoComment[posVar-1]=='>'
        || m_BufferNoComment[posVar-1]=='\n'
        || m_BufferNoComment[posVar-1]==' '
        || m_BufferNoComment[posVar-1]=='('
        || m_BufferNoComment[posVar-1]=='['
        )
        &&(
        m_BufferNoComment[posVar+v.size()]=='.'
        || m_BufferNoComment[posVar+v.size()]=='>'
        || m_BufferNoComment[posVar+v.size()]=='\n'
        || m_BufferNoComment[posVar+v.size()]==' '
        || m_BufferNoComment[posVar+v.size()]=='('
        || m_BufferNoComment[posVar+v.size()]=='['
        || m_BufferNoComment[posVar+v.size()]==')'
        || m_BufferNoComment[posVar+v.size()]==']'
        )
        )
        {

      size_t i = posVar-1;
      while(i>0)
        {
        if(m_BufferNoComment[i]==' '
         || m_BufferNoComment[i]=='('
         || m_BufferNoComment[i]=='['
         || m_BufferNoComment[i]=='\n'
         || m_BufferNoComment[i]=='!'
         || m_BufferNoComment[i]=='{'
         || m_BufferNoComment[i]==';'
         || m_BufferNoComment[i]=='<'
         || m_BufferNoComment[i]=='*'
         )
          {
          break;
          }
        i--;
        }

      std::string var = m_BufferNoComment.substr(i+1,posVar-i+v.size()-1);
      
      bool showError = true;

      // Check if this a macro
      if(this->GetLineNumber(posVar,true) == p+1)
        {
        showError = false;
        }
      else
        {
        std::string line = this->GetLine(this->GetLineNumber(posVar,true)-1);
        if(line.find("Macro") != std::string::npos)
          {
          showError = false;
          }
        }

      // Check the regex
      if(showError && !regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(posVar,true);
        error.line2 = error.line;
        error.number = VARS;
        error.description = "variable (" + var + ") doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
        }
      posVar = m_BufferNoComment.find(v,posVar+1);
      }
    it++;
    }
  return !hasError;
}

/** Find the first ivar in the source code */
std::string Parser::FindVariable(std::string & buffer, size_t start, size_t end,size_t & pos)
{
  size_t posSemicolon = buffer.find(";",start);
  while(posSemicolon != std::string::npos && posSemicolon<end)
    {
    // We try to find the word before that
    size_t i = posSemicolon-1;
    bool inWord = true;
    bool first = false;
    std::string ivar = "";
    while(i!=std::string::npos && inWord)
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
    while(i != std::string::npos)
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

    if(i != std::string::npos)
      {
      subphrase = buffer.substr(i+1,posSemicolon-i-1);
      }

    if( (subphrase.find("=") == std::string::npos)
      && (subphrase.find("(") == std::string::npos)
      && (subphrase.find("typedef") == std::string::npos)
      && (subphrase.find("}") == std::string::npos)
      && (subphrase.find("friend") == std::string::npos)
      && (subphrase.find("class") == std::string::npos)
      && (subphrase.find("return") == std::string::npos)
      && (subphrase.find("\"") == std::string::npos)
      && (subphrase.find("<<") == std::string::npos)
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

  pos = std::string::npos;
  return "";
}

} // end namespace kws
