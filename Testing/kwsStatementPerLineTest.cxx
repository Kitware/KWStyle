/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
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
  parser.Check("StatementPerLine","1");

  // Test for bad syntax
  std::cout << "Test for bad syntax: ";
  kws::Parser::ErrorVectorType errors = parser.GetErrors();
  if (!errors.empty()) {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].description << std::endl;
      }
  } else {
    std::cout << "[FAILED] for buffer: " << buffer << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "[PASSED]" << std::endl;

  // Test for good syntax
  buffer = "Test1();\r\nTest2();";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("StatementPerLine","1");

  std::cout << "Test for good syntax: ";
  errors = parser.GetErrors();
  if (!errors.empty()) {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].description << std::endl;
      }
    std::cout << "[FAILED] for buffer: " << buffer << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "[PASSED]" << std::endl;

  // Test for good syntax
  buffer = "for(int i=0;i<3;i++)";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("StatementPerLine","1");

  std::cout << "Test for good syntax: ";
  errors = parser.GetErrors();
  if (!errors.empty()) {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].description << std::endl;
      }
    std::cout << "[FAILED] for buffer: " << buffer << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "[DONE]" << std::endl;

  return 0;
}
