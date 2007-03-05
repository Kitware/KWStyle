/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsGenerator.h

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __kwsGenerator_h
#define __kwsGenerator_h

#include "kwsParser.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <kwssys/SystemTools.hxx>

namespace kws
{
const char ErrorColor[NUMBER_ERRORS][8] = {
  {'#','F','F','C','C','3','3','\0'},
  {'#','F','F','C','C','6','6','\0'},
  {'#','F','F','F','F','6','6','\0'},
  {'#','6','6','6','6','C','C','\0'},
  {'#','F','F','F','F','6','6','\0'},
  {'#','6','6','6','6','C','C','\0'},
  {'#','9','9','F','F','C','C','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','6','6','6','6','C','C','\0'},
  {'#','F','F','C','C','3','3','\0'},
  {'#','F','F','C','C','3','3','\0'},
  {'#','F','F','C','C','3','3','\0'},
  {'#','F','F','9','9','C','C','\0'},
  {'#','F','F','9','9','C','C','\0'},
  {'#','F','F','3','3','0','0','\0'},
  {'#','0','0','a','a','0','0','\0'},
  {'#','6','6','0','0','3','3','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','6','6','C','C','C','C','\0'},
  {'#','6','6','6','6','C','C','\0'},
  {'#','0','0','0','0','F','F','\0'},
  {'#','0','0','0','0','F','F','\0'},
  {'#','D','4','D','0','C','8','\0'},
  {'#','F','F','F','F','C','C','\0'},
  {'#','9','9','C','C','C','C','\0'},
  {'#','C','C','C','C','F','F','\0'},
  {'#','9','9','C','C','C','C','\0'},
  {'#','F','F','3','3','0','0','\0'},
  {'#','9','9','C','C','C','C','\0'},
  {'#','F','F','3','3','0','0','\0'}
  };

class Generator
{
public:

  typedef std::vector<Parser> ParserVectorType;

  /** Constructor */
  Generator();

  /** Destructor */
  ~Generator();

  /** Set the buffer to analyze */
  void SetParser(ParserVectorType* parsers) 
    {
    m_Parsers = parsers;
    }
 
  /** Generate the HTML files */
  bool GenerateHTML(const char* dir,bool showAllErrors = true);

  /** Export the HTML report */
  void ExportHTML(std::ostream & output);

  /** Generate dart files */
  bool GenerateDart(const char* dir,int maxerror = -1,
                    bool group = false,std::string url="",
                    double time=0);

  /** Set the configuration file */
  void ReadConfigurationFile(const char* configFile);

private:

  bool CreateHeader(std::ostream * file,const char* title);
  bool CreateFooter(std::ostream * file);
  bool GenerateMatrix(const char* dir,bool showAllErrors);
  bool GenerateDescription(const char* dir);

  ParserVectorType* m_Parsers;
  std::string       m_ProjectTitle;
  std::string       m_ProjectLogo;
  std::string       m_KWStyleLogo;
  unsigned int      m_MaxDirectoryDepth;
  unsigned int      m_ErrorThreshold;
};

} // end namespace kws

#endif
