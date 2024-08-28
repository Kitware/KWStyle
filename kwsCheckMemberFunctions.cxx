/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckMemberFunctions.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the member function implementation of the class are correct */
bool Parser::CheckMemberFunctions(const char* regEx,unsigned long maxLength)
{
  m_TestsDone[MEMBERFUNCTION_REGEX] = true;
  m_TestsDescription[MEMBERFUNCTION_REGEX] = "Member functions should match regular expression: ";
  m_TestsDescription[MEMBERFUNCTION_REGEX] += regEx;

  if(maxLength>0)
    {
    m_TestsDone[MEMBERFUNCTION_LENGTH] = true;
    m_TestsDescription[MEMBERFUNCTION_LENGTH] = "Member functions must not exceed: ";
    m_TestsDescription[MEMBERFUNCTION_LENGTH] += std::string(maxLength);
    m_TestsDescription[MEMBERFUNCTION_LENGTH] += " lines";
    }

  // First we need to find the parameters
  bool hasError = false;

  kwssys::RegularExpression regex(regEx);

  // Construct the list of classnames to check
  std::vector<std::string> classes;

  // We first read the .h if any and not a .h file
  if(kwssys::SystemTools::GetFilenameExtension(m_Filename.c_str()) != ".h")
    {
    std::string headerfile = kwssys::SystemTools::GetFilenamePath(m_Filename.c_str());
    //headerfile += "/";
    headerfile += kwssys::SystemTools::GetFilenameWithoutExtension(m_Filename.c_str());
    headerfile += ".h";

    if(kwssys::SystemTools::FileExists(headerfile.c_str()))
      {
      // We open the file
      std::ifstream file;
      file.open(headerfile.c_str(), std::ios::binary | std::ios::in);
      if(!file.is_open())
        {
        std::cout << "Cannot open file: " << headerfile.c_str() << std::endl;
        return true;
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

      size_t classpos = this->GetClassPosition(0,buffer);
      while(classpos!=std::string::npos)
        {
        std::string classname = this->FindPreviousWord(classpos,false,buffer);
        if(buffer[classpos-1] != ' '
          && m_BufferNoComment[classpos-1] != '\n')
          {
          classname = "";
          size_t i=classpos-1;
          while(i!=std::string::npos && buffer[i] != ' ')
            {
            classname = buffer[i]+classname;
            i--;
            }
          }
        classes.push_back(classname);
        classpos = this->GetClassPosition(classpos+1,buffer);
        }
      }
    }

  // Check the current file for any classes
  // And check the name of the current files
  size_t classpos = this->GetClassPosition(0);
  while(classpos!=std::string::npos)
    {
    size_t current;
    std::string memberFunction =
            this->FindMemberFunction(m_BufferNoComment,classpos,
                                     this->FindEndOfClass(classpos+1),current);

    std::string classname = this->FindPreviousWord(classpos);

    if(m_BufferNoComment[classpos-1] != ' '
      && m_BufferNoComment[classpos-1] != '\n'
      )
      {
      classname = "";
      size_t i=classpos-1;
      while(i!=std::string::npos && m_BufferNoComment[i] != ' ')
        {
        classname = m_BufferNoComment[i]+classname;
        i--;
        }
      }
    std::string destructor = "~";
    destructor += classname;

    while(current!=std::string::npos)
      {
      // if the member function is a constructor or destructor we ignore
      if(memberFunction != classname
        && memberFunction != destructor
        )
        {
        if(!regex.find(memberFunction))
          {
          Error error;
          error.line = this->GetLineNumber(current,true);
          error.line2 = error.line;
          error.number = MEMBERFUNCTION_REGEX;
          error.description = "member function (" + memberFunction + ") doesn't match regular expression";
          m_ErrorList.push_back(error);
          hasError = true;
          }

        // Check the size of the current memberFunction
        if(maxLength>0 && current!=std::string::npos)
          {
          size_t open = m_BufferNoComment.find("{",current);
          size_t semicolon = m_BufferNoComment.find(";",current);
          if(semicolon>open)
            {
            size_t close = this->FindClosingChar('{','}',open,true);
            size_t lopen = this->GetLineNumber(open);
            size_t lclose = this->GetLineNumber(close);
            if((open!=std::string::npos) && (close!=std::string::npos) && (lclose-lopen>maxLength))
              {
              Error error;
              error.line = this->GetLineNumber(current,true);
              error.line2 = error.line;
              error.number = MEMBERFUNCTION_LENGTH;
              error.description = "function (" + memberFunction + ") has too many lines: ";
              constexpr size_t length = 10;
              char* temp = new char[length];
              snprintf(temp,length,"%zd",lclose-lopen);
              error.description += temp;
              error.description += " (";
              snprintf(temp,length,"%ld",maxLength);
              error.description += temp;
              error.description += ")";
              m_ErrorList.push_back(error);
              hasError = true;
              delete [] temp;
              }
            }
          }
        }
      memberFunction = this->FindMemberFunction(m_BufferNoComment,current+1,this->FindEndOfClass(classpos+1),current);
      }
    classes.push_back(classname);
    classpos = this->GetClassPosition(classpos+1);
    }

  // Do the checking for the implementation file
  std::vector<std::string>::const_iterator it = classes.begin();
  while(it != classes.end())
    {
    std::string classname = (*it);

    // Search all the classname::
    size_t pos = m_BufferNoComment.find(classname,0);
    while(pos != std::string::npos)
      {
      // look for ::
      size_t pos2 = m_BufferNoComment.find("::",pos);
      bool valid = false;
      if(pos2 != std::string::npos)
        {
        valid = true;
        for(size_t i=pos+classname.size();i<pos2;i++)
          {
          if(m_BufferNoComment[i] != ' ' && m_BufferNoComment[i] != '\n' && m_BufferNoComment[i] != '\r')
            {
            valid = false;
            break;
            }
          }
        }

      // if we have a match we look for the name
      if(valid)
        {
        size_t i=pos2+2;
        while(i<m_BufferNoComment.size() &&
             (m_BufferNoComment[i] == ' '
             || m_BufferNoComment[i] == '\n'
             || m_BufferNoComment[i] == '\r'))
          {
          i++;
          }

       size_t pos3 = m_BufferNoComment.find("(",i);
       if(pos3 != std::string::npos)
         {
         std::string functionName = m_BufferNoComment.substr(i,pos3-i);
         std::string destructor = "~";
         destructor += classname;

         std::string functionLine = this->GetLine(this->GetLineNumber(i,true)-1);

         // if the member function is a constructor or destructor we ignore
         if(functionName != classname
           && functionName != destructor
           && functionLine.find("typedef ") == std::string::npos
           )
           {
           if(!regex.find(functionName))
             {
             Error error;
             error.line = this->GetLineNumber(i,true);
             error.line2 = error.line;
             error.number = MEMBERFUNCTION_REGEX;
             error.description = "function (" + functionName + ") doesn't match regular expression";
             m_ErrorList.push_back(error);
             hasError = true;
             }

           if(maxLength>0)
             {
             size_t open = m_BufferNoComment.find("{",i);
             size_t close = this->FindClosingChar('{','}',open,true);
             size_t lopen = this->GetLineNumber(open);
             size_t lclose = this->GetLineNumber(close);
             if((open!=std::string::npos) && (close!=std::string::npos)
                && (lclose-lopen>maxLength))
               {
               Error error;
               error.line = this->GetLineNumber(pos3,true);
               error.line2 = error.line;
               error.number = MEMBERFUNCTION_LENGTH;
               error.description = "function (" + functionName + ") has too many lines: ";
               constexpr size_t length = 10;
               char* temp = new char[length];
               snprintf(temp,length,"%zd",lclose-lopen);
               error.description += temp;
               error.description += " (";
               snprintf(temp,length,"%ld",maxLength);
               error.description += temp;
               error.description += ")";
               m_ErrorList.push_back(error);
               hasError = true;
               delete [] temp;
               }
             }
           }
         }
        }
      pos = m_BufferNoComment.find(classname,pos+1);
      }
    it++;
    }

  return !hasError;
}

/** Find the first member function in the source code
 *  Member functions are defined as int myfunction(); */
std::string Parser::FindMemberFunction(std::string & buffer, size_t start, size_t end,size_t& pos)
{
  size_t posSemicolon = buffer.find(";",start);
  while(posSemicolon != std::string::npos && posSemicolon<end)
    {
    // We check that we don't have the keyword __attribute__
    std::string line = this->GetLine(this->GetLineNumber(posSemicolon,true)-1);
    if((line.find("_attribute_") != std::string::npos)
      || (line.find(" operator") != std::string::npos)
      )
      {
      posSemicolon = buffer.find(";",posSemicolon+1);
      continue;
      }

    // We try to find a (
    size_t i=posSemicolon-1;
    bool inFunction = true;
    while(i != std::string::npos && i>=start && inFunction)
      {
      if(buffer[i] == ')')
        {
        size_t close = this->FindClosingChar(')','(',i,true);
        if(close>0 && !this->IsInFunction(close,buffer.c_str()))
          {
          i = close;
          inFunction = false;
          break;
          }
        }
      i--;
      }

    pos = i-1;

    // If we have a match we extract the word
    if(pos != std::string::npos && pos>start && pos<end)
      {
      std::string functionName = "";
      bool inWord = false;
      size_t index=pos;
      for(;index!=std::string::npos && index>start;index--)
        {
        if(buffer[index] != ' ' && buffer[index] != '\t'
           && buffer[index] != '\r' && buffer[index] != '\n' && buffer[index] != '*' && buffer[index] != '&')
          {
          inWord = true;
          functionName = buffer[index]+functionName;
          }
        else if(inWord)
          {
          break;
          }
        }
      // Check that this is not a #define (tricky)
      std::string functionLine = this->GetLine(this->GetLineNumber(i,true)-1);
      if(functionLine.find("#define") == std::string::npos
         && functionLine.find("_attribute_") == std::string::npos
         && functionLine.find(" operator") == std::string::npos
         && functionLine.find("friend ") == std::string::npos)
        {
        // If we have a class definition: Test():Base
        // we return the correct
        size_t posf = functionName.find("(",0);
        if(posf != std::string::npos)
          {
          functionName = functionName.substr(0,posf);
          }
        posf = functionName.find(":",0);
        if(posf != std::string::npos)
          {
          functionName = functionName.substr(0,posf);
          }

        // If the function name is void we skip because it means
        // this is a member variable function (see bug 5086)
        if(functionName != "void")
          {
          return functionName;
          }
        }
      }
    posSemicolon = buffer.find(";",posSemicolon+1);
    }

  pos = std::string::npos;
  return "";
}

} // end namespace kws
