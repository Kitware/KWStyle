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
bool Parser::CheckMemberFunctions(const char* regEx)
{
  m_TestsDone[MEMBERFUNCTION_REGEX] = true;
  m_TestsDescription[MEMBERFUNCTION_REGEX] = "Member functions should match regular expression: ";
  m_TestsDescription[MEMBERFUNCTION_REGEX] += regEx;

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
      
     long int classpos = this->GetClassPosition(0,buffer);
      while(classpos!=-1)
        {
        std::string classname = this->FindPreviousWord(classpos,false,buffer);
        if(buffer[classpos-1] != ' '
          && m_BufferNoComment[classpos-1] != '\n')
          {
          classname = "";
          long int i=classpos-1;
          while(i>0 && buffer[i] != ' ')
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
  long int classpos = this->GetClassPosition(0);
  while(classpos!=-1)
    {
    long int current;
    std::string memberFunction = this->FindMemberFunction(m_BufferNoComment,classpos,this->FindEndOfClass(classpos+1),current);
    
    std::string classname = this->FindPreviousWord(classpos);
        
    if(m_BufferNoComment[classpos-1] != ' ' 
      && m_BufferNoComment[classpos-1] != '\n'
      )
      {
      classname = "";
      long int i=classpos-1;
      while(i>0 && m_BufferNoComment[i] != ' ')
        {
        classname = m_BufferNoComment[i]+classname;
        i--;
        }
      }    
    std::string destructor = "~";
    destructor += classname;

    while(current!=-1)
      {
      // if the member function is a constructor or destructor we ignore
      if(memberFunction != classname
        && memberFunction != destructor
        && !regex.find(memberFunction)
        )
        {
        Error error;
        error.line = this->GetLineNumber(current,true);
        error.line2 = error.line;
        error.number = MEMBERFUNCTION_REGEX;
        error.description = "function (" + memberFunction + ") doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
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
    long int pos = m_BufferNoComment.find(classname,0);
    while(pos != -1)
      {
      // look for ::
      long int pos2 = m_BufferNoComment.find("::",pos);
      bool valid = false;
      if(pos2>0)
        {
        valid = true;
        for(long int i=pos+classname.size();i<pos2;i++)
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
        unsigned long i=pos2+2;
        while(i<m_BufferNoComment.size() && 
             (m_BufferNoComment[i] == ' ' 
             || m_BufferNoComment[i] == '\n' 
             || m_BufferNoComment[i] == '\r'))
          {
          i++;
          }
       
       long int pos3 = m_BufferNoComment.find("(",i);
       if(pos3 != -1)
         {
         std::string functionName = m_BufferNoComment.substr(i,pos3-i);
         std::string destructor = "~";
         destructor += classname;
         // if the member function is a constructor or destructor we ignore
         if(functionName != classname
           && functionName != destructor
           && !regex.find(functionName)
           )
           {
           Error error;
           error.line = this->GetLineNumber(i,true);
           error.line2 = error.line;
           error.number = MEMBERFUNCTION_REGEX;
           error.description = "function (" + functionName + ") doesn't match regular expression";
           m_ErrorList.push_back(error);
           hasError = true;
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
std::string Parser::FindMemberFunction(std::string & buffer, long int start, long int end,long int& pos)
{
  long int posSemicolon = buffer.find(";",start);
  while(posSemicolon != -1 && posSemicolon<end)
    {

    // We check that we don't have the keyword __attribute__
    std::string line = this->GetLine(this->GetLineNumber(posSemicolon,true)-1);
    if((line.find("_attribute_") != -1)
      || (line.find(" operator") != -1)
      )
      {
      posSemicolon = buffer.find(";",posSemicolon+1);
      continue;
      }

    // We try to find a (
    long int i=posSemicolon-1;
    bool inFunction = true;
    while(i>=start && inFunction)
      {
      if(buffer[i] == '(' && !this->IsInFunction(i,buffer.c_str()))
        {
        inFunction = false;
        break;
        }
      i--;
      }
     pos = i-1;
     

     // If we have a match we extract the word
     if(pos>start)
       {
       std::string functionName = "";
       bool inWord = false;
       long int i=pos;
       for(i;i>start;i--)
         {
         if(buffer[i] != ' ' && buffer[i] != '\r' && buffer[i] != '\n')
           {
           inWord = true;
           functionName = buffer[i]+functionName;
           }
         else if(inWord)
           {
           break;
           }
         }

       // Check that this is not a #define (tricky)
       std::string functionLine = this->GetLine(this->GetLineNumber(i,true)-1);
       if(functionLine.find("#define") == -1)
         {
         return functionName;
         }
       }
    posSemicolon = buffer.find(";",posSemicolon+1);
    }

  pos = -1;  
  return "";
}


} // end namespace kws
