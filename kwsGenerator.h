/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsGenerator.h
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
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
#include <itksys/SystemTools.hxx>

namespace kws
{
const char ErrorColor[NUMBER_ERRORS][8] = {
  {'#','F','F','C','C','3','3','\0'},
  {'#','9','9','3','3','C','C','\0'},
  {'#','F','F','F','F','6','6','\0'},
  {'#','9','9','F','F','C','C','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','6','6','6','6','C','C','\0'},
  {'#','F','F','C','C','3','3','\0'},
  {'#','F','F','C','C','3','3','\0'},
  {'#','F','F','9','9','C','C','\0'},
  {'#','F','F','9','9','C','C','\0'},
  {'#','F','F','3','3','0','0','\0'},
  {'#','0','0','a','a','0','0','\0'},
  {'#','6','6','0','0','3','3','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','9','9','C','C','F','F','\0'},
  {'#','9','9','C','C','F','F','\0'}
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
  bool GenerateHTML(const char* dir);

private:

  bool CreateHeader(std::ofstream * file,const char* title);
  bool CreateFooter(std::ofstream * file);
  bool GenerateMatrix(const char* dir);
  bool GenerateDescription(const char* dir);

  ParserVectorType* m_Parsers;

};

} // end namespace kws

#endif
