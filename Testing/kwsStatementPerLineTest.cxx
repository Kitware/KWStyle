/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    kwsStatementPerLineTest.cxx
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

int kwsStatementPerLineTest(int, char* [] )
{
  std::string buffer;
  buffer = "Test1(); Test2();";

  kws::Parser parser;
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("StatementPerLine","1");

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
  buffer = "Test1();\r\nTest2();";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("StatementPerLine","1");

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
  buffer = "for(int i=0;i<3;i++)";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("StatementPerLine","1");

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


  std::cout << "[DONE]" << std::endl;
  
  return 0;
}
