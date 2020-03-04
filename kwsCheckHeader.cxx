/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckHeader.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include <string.h> // strlen

namespace kws {

/** Check header given a template filename 
 *  The template should have '<NA>\n' tag to avoid checking the rest of the line
 *  or <NA> to skip a word.
 *  The header should also be at the beginning of the file */
bool Parser::CheckHeader(const char* filename, bool considerSpaceEOL,bool useCVS)
{
  m_TestsDone[HEADER] = true;
  char* val = new char[255];
  sprintf(val,"The header should respect the template");
  m_TestsDescription[HEADER] = val;
  delete [] val;

  bool hasError = false;
  if(!filename)
    {
    std::cout << "CheckHeader(): Please specify an header file or a directory containing headers" << std::endl;
    return false;
    }

  std::vector<std::string> fileNames;
  
  // if there is no space in the name (i.e on linux) we remove the
  // '"' if any. 
  std::string headerFilename = filename;
  if(headerFilename.find(' ') == std::string::npos)
    {
    headerFilename = "";
    for(unsigned long j=0;j<strlen(filename);j++)
      {
      if(filename[j] != '\"')
        {
        headerFilename+=filename[j];
        }
      }
    }

  // Remove quotes if any on linux
  #ifndef WIN32
    if(headerFilename[0] == '"')
      {
      headerFilename = headerFilename.substr(1,headerFilename.size()-2);
      }
  #endif

  // Check if we have a directory or header
  kwssys::Directory directory;
  if(directory.Load(headerFilename.c_str()))
    {
    std::string dirname = headerFilename;
    if(dirname[dirname.size()-1] != '/')
      {
      dirname += "/";
      }

    for(unsigned int i=0;i<directory.GetNumberOfFiles();i++)
      {
      std::string file = directory.GetFile(i);
      std::string fullpath = dirname+file;
      
      if(file!=".." && file!="." && file!="CVS"&& file!= ".svn")
        {
        fileNames.push_back(dirname+file);
        }
      }
    }
  else
    {
    fileNames.push_back(headerFilename);
    }

  // Create a temporary vector of errors
  std::vector<ErrorVectorType> tempErrors;

  std::vector<std::string>::const_iterator itFilename = fileNames.begin();  
  while(itFilename != fileNames.end())
    {
    hasError = false;

    // Read the header file
    std::ifstream file;
    file.open((*itFilename).c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open header file: " << (*itFilename).c_str() << std::endl;
      return false;
      }

    file.seekg(0,std::ios::end);
    unsigned long fileSize = file.tellg();
    file.seekg(0,std::ios::beg);

    char* buf = new char[fileSize+1];
    file.read(buf,fileSize);
    buf[fileSize] = 0;
    std::string buffer(buf);
    buffer.resize(fileSize);
    file.close();
    delete [] buf;

    this->ConvertBufferToWindowsFileType(buffer);
    
    ErrorVectorType  tempErrorVector;

    // Check the file char by char
    std::string::const_iterator ith = buffer.begin();
    std::string::const_iterator it = m_Buffer.begin();

    unsigned int pos = 0;
    unsigned int posh = 0;
    int line = -1;


    while((ith != buffer.end()) && (it != m_Buffer.end()))
      {
      // if we have cvs
      if((*ith == '$') && useCVS)
        {
        ith++;
        posh++;
        it++;
        pos++;
        while(((*ith) != '$') && (ith != buffer.end()))
          {
          ith++;
          posh++;
          }
        while(((*it) != '$') && (it != m_Buffer.end()))
          {
          pos++;
          it++;
          }
        //continue;
        }
       
      if((*it) != (*ith))
        {
        // Check if we have a <NA> tag
        if((*ith) == '<')
          {
          auto pos2 = static_cast<long int>(buffer.find("<NA>", posh));
          auto pos3 = static_cast<long int>(buffer.find("<NA>\n", posh));

          // We skip the line
          if(pos3 == static_cast<long int>(posh))
            {
            while(((*ith) != '\n') && (ith != buffer.end()))
              {
              ith++;
              posh++;
              }
            while(((*it) != '\n') && (it != m_Buffer.end()))
              {
              pos++;
              it++;
              }
            continue;
            }
          // if we have the tag we skip the word
          else if(pos2 == static_cast<long int>(posh))
            {
            while(((*ith) != ' ') && (ith != buffer.end()))
              {
              ith++;
              posh++;
              }
            while(((*it) != ' ') && (it != m_Buffer.end()))
              {
              pos++;
              it++;
              }
            continue;
            }
          }
        // if we should not check the spaces at the end of line
        else if( (!considerSpaceEOL)
          && ((*ith == ' ') || (*it == ' '))
          )
          {
          // search if we are effectively at the end of the line
          bool isAtEnd = true;
          if(*it == ' ')
            {
            std::string::const_iterator ittemp = it;
            while((ittemp != m_Buffer.end()) && ((*ittemp) != '\n'))
              {
              if(*ittemp != ' ')
                {
                isAtEnd = false;
                break;
                }
              ittemp++;
              }
            }
          else if(*ith == ' ')
            {
            std::string::const_iterator ittemp = ith;
            while((ittemp != buffer.end()) && ((*ittemp) != '\n'))
              {
              if(*ittemp != ' ')
                {
                isAtEnd = false;
                break;
                }
              ittemp++;
              }
            }

          // If we are at the end we skip the line
          if(!isAtEnd)
            {
            while((ith != buffer.end()) && ((*ith) != '\n'))
              {
              ith++;
              posh++;
              }
            while((it != m_Buffer.end()) && ((*it) != '\n'))
              {
              pos++;
              it++;
              }
            continue;
            }
          }

        // Report the error
        //hasError = true;

        // We report the wrong word and the line
        int l = this->GetLineNumber(pos);
        if(l != line)
          {
          line = l;
          // Find the word
          auto poshw = static_cast<long int>(buffer.find(' ', posh));
          auto poshw2 = static_cast<long int>(buffer.find('\n', posh));
          std::string wordh = "";
          if(poshw!= -1 && poshw < poshw2)
            {
            wordh = buffer.substr(posh,poshw-posh);
            }
          else if (poshw2 != -1)
            {
            wordh = buffer.substr(posh,poshw2-posh-1);
            }

          // Find the word
            auto posw = static_cast<long int>(m_Buffer.find(' ', pos));
            auto posw2 = static_cast<long int>(m_Buffer.find('\n', pos));
            std::string word = "";
            if (posw != -1 && posw < posw2) {
              word = m_Buffer.substr(pos, posw - pos);
            }
          else if (poshw2 != -1)
            {
            word = m_Buffer.substr(pos,posw2-pos-1);
            }

          if(word == wordh)
            {
            wordh = "wrong ident";
            }
         
          if(word == " ")
            {
            word = "[space]";
            }
          if(wordh == " ")
            {
            wordh = "[space]";
            }
         if(word == "\r")
            {
            word = "[end of line]";
            }
          if(wordh == "\r")
            {
            wordh = "[end of line]";
            }
          
          if(word[0] == 0)
            {
            word = "[no char]";
            }
          if(wordh[0] == 0)
            {
            wordh = "[no char]";
            }

          Error error;
          error.line = line;
          error.line2 = error.line;
          error.number = HEADER;
          error.description = "Header mismatch: ";
          error.description += word;
          error.description += " (";
          error.description += wordh;
          error.description += ") : ";
          error.description += headerFilename.c_str();
          tempErrorVector.push_back(error);
          hasError = true;

          // We skip that line   
          while((ith != buffer.end()) && ((*ith) != '\n'))
            {
            ith++;
            posh++;
            }
          while((it != m_Buffer.end()) && ((*it) != '\n'))
            {
            pos++;
            it++;
            }
          }
        }

      if(ith != buffer.end())
        {
        posh++;
        ith++;
        }

      if(it != m_Buffer.end())
        {
        pos++;
        it++;
        }
      }

    if(it == m_Buffer.end())
      {
      Error error;
      error.line = 1;
      error.line2 = error.line;
      error.number = HEADER;
      error.description = "The header is incomplete";
      tempErrorVector.push_back(error);
      hasError = true;
      }

    // If we don't have any errors we return the current errors
    if(!hasError)
      {
      m_HeaderFilename = *itFilename;
      ErrorVectorType::const_iterator itErr = tempErrorVector.begin();
      while(itErr != tempErrorVector.end())
        {
        m_ErrorList.push_back(*itErr);
        itErr++;
        }
      return true;
      }

    tempErrors.push_back(tempErrorVector);

    itFilename++;
    } // end loop of filenames

  // We check the less errors we can have
  unsigned int header=0;
  int minErrors = 9999999;
  unsigned int i=0;
  std::vector<ErrorVectorType>::const_iterator itErrV = tempErrors.begin();
  while(itErrV != tempErrors.end())
    {
    if((int)(*itErrV).size() < minErrors)
      {
      header = i;
      minErrors = static_cast<int>((*itErrV).size());
      }
    i++;
    itErrV++;
    }
 
  ErrorVectorType::const_iterator itErr = tempErrors[header].begin();
  while(itErr != tempErrors[header].end())
    {
    m_ErrorList.push_back(*itErr);
    itErr++;
    }

  m_HeaderFilename = fileNames[header];
  return !hasError;
}

} // end namespace kws
