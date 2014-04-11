/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsXMLReader.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsXMLReader.h"
#include <string.h>

namespace kws{

XMLReader::XMLReader()
{
  m_CurrentPos = 0;
}


XMLReader::~XMLReader()
{
}

bool XMLReader::Open(const char* filename)
{
  // Open file for reading
  m_File.open(filename,std::ifstream::binary);
  if ( m_File.fail() )
    {
    return false;
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

  return true;
}

std::string XMLReader::GetValue()
{
  return m_Value;
}

std::string XMLReader::GetTag()
{
  long int begin_tag_start = m_Buffer.find("<",m_CurrentPos);
  if(begin_tag_start == -1)
    {
    m_CurrentPos++;
    m_Value = "";
    return "";
    }

  long int comment_start = m_Buffer.find("<?",m_CurrentPos);
  if(begin_tag_start == comment_start)
    {
    m_CurrentPos+=2;
    return "NA";
    }
  comment_start = m_Buffer.find("</",m_CurrentPos);
  if(begin_tag_start == comment_start)
    {
    m_CurrentPos+=2;
    return "NA";
    }

  comment_start = m_Buffer.find("<!--",m_CurrentPos);
  if(begin_tag_start == comment_start)
    {
    long int comment_end = m_Buffer.find("-->",comment_start);
    if(comment_end!=-1)
      {
      m_CurrentPos=comment_end+3;
      }
    else
      {
      std::cout << "ERROR: Cannot find closing comments in configuration file" << std::endl;
      m_CurrentPos+=2;
      }
    return "NA";
    }




  long int begin_tag_end =  m_Buffer.find(">",begin_tag_start+1);
  m_Tag = m_Buffer.substr(begin_tag_start+1,begin_tag_end-begin_tag_start-1);

  if(!strcmp(m_Tag.c_str(),"Description"))
    {
    m_CurrentPos++;
    return "Description";
    }

  std::string m_EndTag = "</";
  m_EndTag += m_Tag;
  long int end_tag_begin = m_Buffer.find(m_EndTag,begin_tag_end+1);
  if(end_tag_begin == -1)
    {
    std::cout << "XML parsing error, cannot find close tag for "
              << m_Tag.c_str() << std::endl;
    return "";
    }

  m_CurrentPos = end_tag_begin+1;

  std::string value = m_Buffer.substr(begin_tag_end+1,end_tag_begin-begin_tag_end-1);
  m_Value = value;

  // If we have multiple tags inside the actually tag we return the values separated
  // by comas. This assume that the order of the tags is correct and present and
  // that we only have 1 level in the XML tree.
  unsigned int i=0;
  long int pos1 = value.find("<",0);
  while(pos1 != -1)
    {
    if(i>0)
      {
      m_Value += ",";
      }
    long int pos2 = value.find(">",pos1);
    long int pos3 = value.find("<",pos2);

    if(i == 0)
      {
      m_Value = value.substr(pos2+1,pos3-pos2-1);

      }
    else
      {
      m_Value += value.substr(pos2+1,pos3-pos2-1);
      }

    pos1 = value.find("<",pos3+1);
    i++;
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
