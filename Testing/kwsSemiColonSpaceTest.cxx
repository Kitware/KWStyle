/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    kwsSemiColonSpaceTest.cxx
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

int kwsSemiColonSpaceTest(int, char* [] )
{
  std::string buffer;
  buffer = "Test() ;";

  kws::Parser parser;
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");

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

  // Test for bad syntax
  buffer = "Test();;";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");
  std::cout << "Test for bad syntax: ";
  errors = parser.GetErrors();
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
  buffer = "std::string str = \" ;\"; ";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");
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

  // Test for bad syntax
  buffer = "class myclass\n{\nmyfunction(){this is a test};\n\n\n\n};";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");
  std::cout << "Test for bad syntax: ";
  errors = parser.GetErrors();
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

  // Test for good syntax of initializer value return lists
  buffer = "const int spacing{ 1 };";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");

  std::cout << "Test for return value initializer lists: ";
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
  buffer = "class myclass\n{\nmyfunction(){};\n\n\n\n};";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");

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
  buffer = "class myclass\n{\nmyfunction(){test;}\n\n\n\n};";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");

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
  buffer = "Test();";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");

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
  buffer = "std::string str( \"();;\" );";
  //buffer += "}\n";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("SemicolonSpace","0");

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
