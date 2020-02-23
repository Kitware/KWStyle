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

int kwsSwitchCaseTest(int, char* [] )
{
  std::string buffer;
  buffer = "void()\n{\ntest;\n};";

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
  buffer = "void()\n{\n  test;\n};";
  parser.ClearErrors();
  parser.SetBuffer(buffer);
  parser.Check("Indent","SPACE,2,true,true");

  std::cout << "Test for good syntax: ";
  errors = parser.GetErrors();
  if(errors.size() > 0)
    {
    for(unsigned int i=0;i<errors.size();i++)
      {
      std::cout << errors[i].line << " : "
                << errors[i].description << std::endl;
      }
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
    }
  std::cout << "[PASSED]" << std::endl;

  std::cout << "[DONE]" << std::endl;

  return 0;
}
