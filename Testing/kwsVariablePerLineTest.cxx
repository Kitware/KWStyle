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

int kwsVariablePerLineTest(int, char* [] )
{
  std::string buffer;
  buffer = "//This is a test\r\nint a,b, c;";

  kws::Parser parser;
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("VariablePerLine","1");

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
  buffer = "int a;\r\nint b;";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("VariablePerLine","1");

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
  buffer = "int a,b;";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("VariablePerLine","2");

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
  buffer = "template < int t,\r\nint x, int c > foo;";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("VariablePerLine","1");

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

  // Test for good syntax
  buffer = "void func( int t,\r\n  int x, int c, \r\n  int d );";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("LineLength","999");
  parser.Check("VariablePerLine","1");

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
