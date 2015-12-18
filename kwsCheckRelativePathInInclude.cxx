/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckUsingDirectives.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check if the file contains includes with paths that refer to parent directory (ie: #include "../folder/file.h" ) */
bool Parser::CheckRelativePathInInclude(bool forbidRelativePaths)
{
  if(!forbidRelativePaths)
    return true;

  bool hasError = false;
  size_t includePos = m_BufferNoComment.find("#include", 0);
  while( (includePos = m_BufferNoComment.find("#include", includePos)) != std::string::npos)
    {
    if(this->IsBetweenQuote(includePos, false))
      {
      includePos += 8;
      continue;
      }

    size_t newlinePos = m_BufferNoComment.find('\n', includePos);
    size_t parentDirPos = m_BufferNoComment.find("..", includePos);

    if(parentDirPos != std::string::npos && 
        ( (newlinePos != std::string::npos && parentDirPos < newlinePos) || newlinePos == std::string::npos )
      )
      {
      hasError = true;
      Error error;
      error.line = this->GetLineNumber(includePos, true);
      error.line2 = error.line;
      error.description = "#include refers to a parent directory in the path.";
      error.number = RELATIVE_PATH_IN_INCLUDE;
      m_ErrorList.push_back(error);
      }
    includePos += 8;
    }
  return !hasError;
}

} // end namespace kws
