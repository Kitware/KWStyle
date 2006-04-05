/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsXMLReader.h

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _XMLREADER_H
#define _XMLREADER_H

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>

namespace kws{

class XMLReader
{

public:

  XMLReader();
  ~XMLReader();
  int Open(const char* filename);
  std::string GetTag();
  std::string GetCurrentTag();
  std::string GetValue();
  void Close();

  /** Check the file to see if a current tag is defined */
  std::string GetValue(std::string tag);

private:

  std::ifstream m_File;
  std::string   m_Value;
  std::string   m_Tag;
  std::string   m_Buffer;
  unsigned int  m_CurrentPos;

};

} // end namespace

#endif
