/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    kwsVariablePerLineTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "kwsParser.h"

int kwsIndentTest(int, char* [] )
{
  std::string buffer;
  buffer = "int main(int argc, argv)\r\n{\r\n   int a;\r\n}";

  kws::Parser parser;
  parser.SetBuffer(buffer);
  parser.Check("Indent","SPACE,2,true,true");

  // Test for bad syntax
  std::cout << "Test for bad syntax: ";
  kws::Parser::ErrorVectorType errors = parser.GetErrors();
  if(errors.size() > 0)
    {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].description << std::endl;
      }
    }
  else
    {
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test for good syntax
  buffer = "int main(int argc, argv)\r\n{\r\n  int a;\r\n}";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("Indent","SPACE,2,true,true");

  std::cout << "Test for good syntax: ";
  errors = parser.GetErrors();
  if(errors.size() > 0)
    {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].description << std::endl;
      }
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  // Test for good syntax
  buffer = "int main(int argc, argv)\r\n{\r\n  std::string x(\"my string's test\");\r\n  for(;;)\r\n    {\r\n    }\r\n  int a;\r\n}";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("Indent","SPACE,2,true,true");

  std::cout << "Test for good syntax: ";
  errors = parser.GetErrors();
  if(errors.size() > 0)
    {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].description << std::endl;
      std::cout << "Line = " << errors[i].line << std::endl;
      }
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  return 0;
}
