/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckComments.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$
  Author:    Julien Jomier

  Copyright (c) 2002 CADDLab @ UNC. All rights reserved.
  See itkUNCCopyright.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Check the comments
 * The comment definition should be set before CheckIndent() to get the correct indentation
 * for the comments. */
bool Parser::CheckComments(const char* begin,const char* middle,const char* end)
{

  m_TestsDone[NAMESPACE] = true;

  bool hasError = false;
  // Set the ivars for the indent checking
  m_CommentBegin = begin;
  m_CommentMiddle = middle;
  m_CommentEnd = end;

  return !hasError;
}

} // end namespace kws
