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

#include "kwsParser.h"

int kwsBadCharactersTest(int, char* [] )
{
  constexpr char nonUTF8[] = {static_cast<char>(0xE4),
                              static_cast<char>(0xFC),
                              static_cast<char>(0xF6),
                              static_cast<char>(0xDF),
                              0};

  // 1) String with bad characters only in the code.
  std::string buffer = "void()\n{\ntest";
  buffer += nonUTF8;
  buffer += ";\n}\n";

  // Test for bad characters (including within comments).
  kws::Parser parser;
  parser.SetBuffer(buffer);
  parser.Check("BadCharacters","true");

  std::cout << "Test with bad characters only in code: ";
  kws::Parser::ErrorVectorType errors = parser.GetErrors();
  if (!errors.empty()) {
    for (auto &error : errors) {
      std::cout << error.description << std::endl;
    }
    std::cout << "[PASSED]" << std::endl;
  } else {
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  // 2) String with bad characters only in the comments.
  buffer = "void()\n{\n  //test";
  buffer += nonUTF8;
  buffer += "\n}\n";

  parser.SetBuffer(buffer);

  // 2a) Test for bad characters (including within comments).
  parser.ClearErrors();
  parser.Check("BadCharacters","true");

  std::cout << "Test with bad characters in comments, include comments: ";
  errors = parser.GetErrors();
  if (!errors.empty()) {
    for (auto &error : errors) {
      std::cout << error.description << std::endl;
    }
    std::cout << "[PASSED]" << std::endl;
  } else {
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  // 2b) Test for bad characters (excluding within comments).
  parser.ClearErrors();
  parser.Check("BadCharacters","false");

  std::cout << "Test with bad characters in comments, exclude comments: ";
  errors = parser.GetErrors();
  if (errors.empty()) {
    std::cout << "[PASSED]" << std::endl;
  } else {
    for (auto &error : errors) {
      std::cout << error.description << std::endl;
    }
    std::cout << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "[DONE]" << std::endl;

  return 0;
}
