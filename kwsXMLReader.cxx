/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsXMLReader.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsXMLReader.h"

namespace kws{

XMLReader::XMLReader()
{
  m_CurrentPos = 0;
}


XMLReader::~XMLReader()
{
}

int XMLReader::Open(const char* filename)
{
  // Open file for reading
  m_File.open(filename,std::ifstream::binary);
  if (m_File == NULL)
    {
    return -1;
    }

  m_File.seekg(0,std::ios::end);
  unsigned long fileSize = m_File.tellg();
  m_File.seekg(0,std::ios::beg);

  char* buf = new char[fileSize+1];
  m_File.read(buf,fileSize);
  buf[fileSize] = 0; 
  m_Buffer = buf;
  m_Buffer.resize(fileSize);
  delete [] buf;
   
  return 0;
} 

std::string XMLReader::GetValue()
{
  return m_Value;
}

std::string XMLReader::GetTag()
{
  unsigned long pos = m_Buffer.find("\n",m_CurrentPos);
  if(pos == -1)
    {
    m_CurrentPos++;
    m_Value = "";
    return "";
    }

  std::string line = m_Buffer.substr(m_CurrentPos,pos-m_CurrentPos-1);
  m_CurrentPos = pos+1;
  
  long int begin_tag_start = line.find("<");
  long int begin_tag_end =  line.find(">",begin_tag_start+1);
  long int end_tag_begin = line.find("<",begin_tag_end+1);

  m_Tag = line.substr(begin_tag_start+1,begin_tag_end-begin_tag_start-1);

  if (end_tag_begin != -1)
    {
    m_Value = line.substr(begin_tag_end+1,end_tag_begin-begin_tag_end-1);
    }
  else
    {
    m_Value = "";
    }

  return m_Tag;
}

/** Check the file to see if a current tag is defined */
std::string XMLReader::GetValue(std::string tag)
{
  m_CurrentPos = 0;
  std::string tagt = this->GetTag();
  while((tagt.length()>0)  && (strcmp(tagt.c_str(),tag.c_str())))
    {
    tagt = this->GetTag();
    }

  return m_Value;
}


std::string XMLReader::GetCurrentTag()
{
  return m_Tag;
}

void XMLReader::Close()
{
  m_File.close();
}

} // end namespace
