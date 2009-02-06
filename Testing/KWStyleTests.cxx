/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    KWStyleTests.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsTestMain.h" 


void RegisterTests()
{
  REGISTER_TEST(kwsSemiColonSpaceTest);
  REGISTER_TEST(kwsStatementPerLineTest);
  REGISTER_TEST(kwsVariablePerLineTest);
  REGISTER_TEST(kwsSwitchCaseTest);
  REGISTER_TEST(kwsBadCharactersTest);
  REGISTER_TEST(kwsRunKWStyleTest);
}
