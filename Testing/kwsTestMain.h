
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    kwsTestMain.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

  Portions of this code are covered under the VTK copyright.
  See VTKCopyright.txt or http://www.kitware.com/VTKCopyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

// This file is used to create TestDriver executables
// These executables are able to register a function pointer to a string name
// in a lookup table.   By including this file, it creates a main function
// that calls RegisterTests() then looks up the function pointer for the test
// specified on the command line.
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h> // strcmp
#include <stdlib.h> // atoi
#include "kwssys/SystemTools.hxx"

#define ITK_TEST_DIMENSION_MAX 6

typedef int (*MainFuncPointer)(int , char* [] );
std::map<std::string, MainFuncPointer> StringToTestFunctionMap;

#define REGISTER_TEST(test) \
extern int test(int, char* [] ); \
StringToTestFunctionMap[#test] = test

int RegressionTestImage (const char *, const char *, int);
std::map<std::string,int> RegressionTestBaselines (char *);

void RegisterTests();
void PrintAvailableTests()
{
  std::cout << "Available tests:\n";
  std::map<std::string, MainFuncPointer>::iterator j = StringToTestFunctionMap.begin();
  int i = 0;
  while(j != StringToTestFunctionMap.end())
    {
    std::cout << i << ". " << j->first << "\n";
    ++i;
    ++j;
    }
}

/** Compare the baseline and the testfile */
int Compare(const char* baseline, const char* testfile)
{
  // Load the baseline
  std::ifstream baselineFile;
  baselineFile.open(baseline, std::ios::binary | std::ios::in);
  if(!baselineFile.is_open())
    {
    std::cout << "Cannot open baselineFile for reading: " << baseline << std::endl;
    return 1;
    }
 
  baselineFile.seekg(0,std::ios::end);
  unsigned long fileSize = baselineFile.tellg();
  baselineFile.seekg(0,std::ios::beg);

  char* buf = new char[fileSize+1];
  baselineFile.read(buf,fileSize);
  buf[fileSize] = 0; 
  std::string baselineBuffer(buf);
  baselineBuffer.resize(fileSize);
  delete [] buf; 
  baselineFile.close();

  // Load the testfile
  std::ifstream testfileFile;
  testfileFile.open(testfile, std::ios::binary | std::ios::in);
  if(!testfileFile.is_open())
    {
    std::cout << "Cannot open testfileFile for reading: " << testfile << std::endl;
    return 1;
    }

  testfileFile.seekg(0,std::ios::end);
  fileSize = testfileFile.tellg();
  testfileFile.seekg(0,std::ios::beg);

  buf = new char[fileSize+1];
  testfileFile.read(buf,fileSize);
  buf[fileSize] = 0; 
  std::string testfileBuffer(buf);
  testfileBuffer.resize(fileSize);
  delete [] buf; 
  testfileFile.close();

  // Go through the testfile and check if we have the same errors than the
  // baseline
  long int posError = testfileBuffer.find("<Error>");
  long int posErrorBase = testfileBuffer.find("<Error>");;

  while(posError != -1)
    {
    long int posErrorEnd = testfileBuffer.find("</Error>",posError);
    std::string subError = testfileBuffer.substr(posError,posErrorEnd-posError);
    long int line1a = subError.find("<SourceLineNumber1>");
    long int line1b = subError.find("</SourceLineNumber1>");
    long int line2a = subError.find("<SourceLineNumber2>");
    long int line2b = subError.find("</SourceLineNumber2>");
    long int errorNumbera = subError.find("<ErrorNumber>");
    long int errorNumberb = subError.find("</ErrorNumber>");   
    unsigned int l1 = atoi(subError.substr(line1a+19,line1b-line1a-19).c_str());
    unsigned int l2 = atoi(subError.substr(line2a+19,line2b-line2a-19).c_str());
    unsigned int errorNumber = atoi(subError.substr(errorNumbera+13,errorNumberb-errorNumbera-13).c_str());

    // Check the baseline
    if(posErrorBase == -1)
      {
      std::cout << "Error not found" << std::endl;
      return 1;
      }
    long int posErrorEndBase = baselineBuffer.find("</Error>",posErrorBase);
    std::string subErrorBase = baselineBuffer.substr(posErrorBase,posErrorEndBase-posErrorBase);
    long int line1aBase = subErrorBase.find("<SourceLineNumber1>");
    long int line1bBase = subErrorBase.find("</SourceLineNumber1>");
    long int line2aBase = subErrorBase.find("<SourceLineNumber2>");
    long int line2bBase = subErrorBase.find("</SourceLineNumber2>");
    long int errorNumberaBase = subErrorBase.find("<ErrorNumber>");
    long int errorNumberbBase = subErrorBase.find("</ErrorNumber>");   
    unsigned int l1Base = atoi(subErrorBase.substr(line1aBase+19,line1bBase-line1aBase-19).c_str());
    unsigned int l2Base = atoi(subErrorBase.substr(line2aBase+19,line2bBase-line2aBase-19).c_str());
    unsigned int errorNumberBase = atoi(subErrorBase.substr(errorNumberaBase+13,errorNumberbBase-errorNumberaBase-13).c_str());

    if(l1 != l1Base || l2 != l2Base || errorNumber!=errorNumberBase)
      {
      std::cout << "Errors differ:" << std::endl;
      std::cout << "l1 = " << l1 << " v.s. "  << l1Base << std::endl;
      std::cout << "l2 = " << l2 << " v.s. "  << l2Base << std::endl;
      std::cout << "errorNumber = " << errorNumber << " v.s. "  << errorNumberBase << std::endl;
      return 1;
      }

    posError = testfileBuffer.find("<Error>",posError+1);
    posErrorBase = baselineBuffer.find("<Error>",posErrorBase+1);
    }
  
  // Check that we don't have any other errors in the baseline
  if(posErrorBase != -1)
    {
    std::cout << "The baseline as more errors than the test" << std::endl;
    return 1;
    }

  return 0;
}

int main(int ac, char* av[] )
{
  char *baselineFilename = NULL;
  char *testFilename = NULL;

  RegisterTests();
  std::string testToRun;
  if(ac < 2)
    {
    PrintAvailableTests();
    std::cout << "To run a test, enter the test number: ";
    int testNum = 0;
    std::cin >> testNum;
    std::map<std::string, MainFuncPointer>::iterator j = StringToTestFunctionMap.begin();
    int i = 0;
    while(j != StringToTestFunctionMap.end() && i < testNum)
      {
      ++i;
      ++j;
      }
    if(j == StringToTestFunctionMap.end())
      {
      std::cerr << testNum << " is an invalid test number\n";
      return -1;
      }
    testToRun = j->first;
    }
  else
    {if (ac > 3 && strcmp(av[1], "--compare") == 0)
      {
      baselineFilename = av[2];
      testFilename = av[3];
      av += 3;
      ac -= 3;
      }
    testToRun = av[1];
    }
  std::map<std::string, MainFuncPointer>::iterator j = StringToTestFunctionMap.find(testToRun);
  if(j != StringToTestFunctionMap.end())
    {
    MainFuncPointer f = j->second;
    int result;
    try
      {
      // Invoke the test's "main" function.
      result = (*f)(ac-1, av+1);

      // Make a list of possible baselines
      if (baselineFilename && testFilename)
        {
        result = Compare(baselineFilename,testFilename);
        }
      }
    catch(const std::exception& e)
      {
      std::cerr << "Test driver caught an exception:\n";
      std::cerr << e.what() << "\n";
      result = -1;
      }
    catch(...)
      {
      std::cerr << "Test driver caught an unknown exception!!!\n";
      result = -1;
      }
    return result;
    }
  PrintAvailableTests();
  std::cerr << "Failed: " << testToRun << ": No test registered with name " << testToRun << "\n";
  return -1;
}
