/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsParser.h
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __kwsParser_h
#define __kwsParser_h

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>
#include <itksys/RegularExpression.hxx>
#include <algorithm>

namespace kws
{
#define MAX_CHAR 99999999
#define NUMBER_ERRORS 22

typedef enum
  {
  SPACE,
  TAB
  } IndentType;

typedef enum
  {
  LINE_LENGTH = 0,
  IVAR_PUBLIC,
  IVAR_REGEX,
  IVAR_ALIGN,
  SEMICOLON_SPACE,
  DECL_ORDER,
  EOF_NEW_LINE,
  TABS,
  SPACES,
  INDENT,
  HEADER,
  NDEFINE,
  TYPEDEF_REGEX,
  TYPEDEF_ALIGN,
  NAMESPACE,
  NAMEOFCLASS,
  WRONGCOMMENT,
  MISSINGCOMMENT,
  EMPTYLINES,
  TEMPLATE,
  OPERATOR,
  BLACKLIST
  } ErrorType;

const char ErrorTag[NUMBER_ERRORS][4] = {
   {'L','E','N','\0'},
   {'I','V','P','\0'},
   {'I','V','R','\0'},
   {'I','V','A','\0'},
   {'S','E','M','\0'},
   {'D','C','L','\0'},
   {'E','O','F','\0'},
   {'T','A','B','\0'},
   {'E','S','P','\0'},
   {'I','N','D','\0'},
   {'H','R','D','\0'},
   {'D','E','F','\0'},
   {'T','D','R','\0'},
   {'T','D','A','\0'},
   {'N','M','S','\0'},
   {'N','M','C','\0'},
   {'W','C','M','\0'},
   {'M','C','M','\0'},
   {'E','M','L','\0'},
   {'T','P','L','\0'},
   {'O','P','S','\0'},
   {'B','L','K','\0'}
  };


typedef struct
  {
  unsigned long line; // main line of the error
  unsigned long line2; // second line of error if the error is covering several lines
  unsigned long number;
  std::string description; 
  } Error;

typedef struct
  {
  unsigned long line; // main line of the error
  unsigned long line2; // second line of error if the error is covering several lines
  unsigned long number;
  std::string description; 
  } Info;

typedef struct
  {
  // Position in the file
  unsigned long position;
  // What should be the position in the line of
  // the word w.r.t the previous position 
  int current;
  // What should be the position in the line of
  // the words after w.r.t the previous position 
  int after;
  } IndentPosition;

class Parser
{
public:

  /** Constructor */
  Parser();

  /** Destructor */
  ~Parser();
  
  typedef std::vector<Error> ErrorVectorType;

  /** Set the buffer to analyze */
  void SetBuffer(std::string buffer) 
    {
    m_Buffer = buffer;
    this->ConvertBufferToWindowsFileType(m_Buffer);
    this->RemoveComments();
    }
 
  /** Return the error tag as string given the error number */
  std::string GetErrorTag(unsigned long number) const;

  /** Return the erro vector */
  const ErrorVectorType & GetErrors() const {return m_ErrorList;}

  /** Return the last error message */
  std::string GetLastErrors();

  /** Return the information vector */
  std::string GetInfo();

  /** Check if the file contains tabs */
  bool CheckTabs();

  /** Check the number of succesive empty lines */
  bool CheckEmptyLines(unsigned long max,bool checkEndOfFile=false);

  /** Check the comments
   * The comment definition should be set before CheckIndent() to get the correct indentation
   * for the comments. */
  bool CheckComments(const char* begin,const char* middle,const char* end,
                     bool allowEmptyLineBeforeClass=false);

  /** Check the indent size 
   *  Not in the header file if there is one 
   *  If CheckHeader has been done before CheckIndent and doNotCheckHeader is set to true
   *  then the header is not checked for indent*/
  bool CheckIndent(IndentType,
                   unsigned long size,
                   bool doNotCheckHeader=false,
                   bool allowBlockLine = false,
                   unsigned int maxLength = 81);

  /** Check Operator spaces foo=bar or foo = bar, etc... */
  bool CheckOperator(unsigned int foo, unsigned int bar,bool doNotCheckInsideParenthesis=true);

  /** Check the number of character per line */
  bool CheckLineLength(unsigned long max);

  /** Check if the internal parameters of the class are correct */
  bool CheckInternalVariables(const char* regEx,bool alignement = true);

  /** Check if the typedefs of the class are correct */
  bool CheckTypedefs(const char* regEx, bool alignment = true,
                     unsigned int maxLength = 81);

  /** Check the order of the declaration */
  bool CheckDeclarationOrder(unsigned int posPublic, unsigned int posProtected, unsigned int posPrivate);

  /** Check for extra spaces */
  bool CheckExtraSpaces(unsigned long max);

  /** Check the number of space between the end of the declaration
   *  and the semicolon */
  bool CheckSemicolonSpace(unsigned long max);

  /** Check if the end of the file has a new line */
  bool CheckEndOfFileNewLine();

  /** Check header 
   *  Because most of the time the header is checked in cvs we should ignore the $ $*/
  bool CheckHeader(const char* filename,bool considerSpaceEOL = true,bool useCVS=true);

  /** Check if the #ifndef/#define is defined correctly for the class */
  bool CheckIfNDefDefine(const char* match);

  /** Check the first namespace in the file */
  bool CheckNamespace(const char* name,bool doNotCheckMain=true);

  /** Check the templates */
  bool CheckTemplate(const char* regex);

  /** Check if the name of the class is correct */
  bool CheckNameOfClass(const char* name, const char* prefix);

  /** Check that there is no word in the class that matches a word in the black list */
  bool CheckBlackList(const char* filename);

  /** Remove the comments. */
  void RemoveComments();

  /** Clear the error list */
  void ClearErrors() {m_ErrorList.clear();}
  
  /** Clear the info list */
  void ClearInfo() {m_InfoList.clear();}

  /** Set the filename of the file we are checking */
  void SetFilename(const char* filename) {m_Filename = filename;}
  std::string GetFilename() const {return m_Filename;}

  /** Return the number of lines */
  unsigned long GetNumberOfLines() const;

  /** Return the line */
  std::string GetLine(unsigned long i) const;

  /** Return if a test has been performed */
  bool HasBeenPerformed(unsigned int test) const;
 
  /** Return the test description given the error number) */
  std::string GetTestDescription(unsigned int test) const;

  /** Given the name of the check to perform and the default value perform the check */
  bool Check(const char* name, const char* value);

protected:

  /** Convert the file with \r\n instead of \n */ 
  void ConvertBufferToWindowsFileType(std::string & buffer);
  
  /** Check the operator.
   *  \warning This function add an error in the Error list */
  bool FindOperator(const char* op,unsigned int before, unsigned int after,bool doNotCheckInsideParenthesis=true);

  /** Get the class position within the file. This function checks that this is the 
   *  classname */
  long int GetClassPosition(long int position) const;

  /** Return the position in the line given the position in the text */ 
  unsigned long GetPositionInLine(long pos);

  /** Find an ivar in the source code */
  std::string Parser::FindInternalVariable(long int start, long int end,long int& pos);

  /** Find a typedef in the source code */
  std::string Parser::FindTypedef(long int start, long int end,long int& pos,long int & beg,long int & typedefpos);

  /** Reduces multiple spaces in buffer to one. */
  void ReduceMultipleSpaces(std::string & buffer);

  /** Removes all val chars from string. */
  void RemoveChar(std::string & buffer, char val) const;

  /** Find the line number in the commented text given the character description */
  long int GetLineNumber(long int pos,bool withoutComments=false) const;

  /** Find the parameters. */
  //void FindAndAddParameters(std::string buffer, XMLDescription &desc, size_t startPos=0) const;

  /** Find the typedefs (only the public ones). */
  //void FindAndAddTypeDefinitions(const std::string & buffer, XMLDescription &desc, size_t startPos=0) const;

  /** Find the name of the filter and templates, etc ... Returns the position 
      within the file where class definition starts. */
  //size_t FindAndAddName(const std::string & buffer, XMLDescription &desc, std::string fileName) const;

  /** Find the previous word given a position */
  std::string FindPreviousWord(long int pos,bool withComments=false) const;

  /** Find the next word given a position */
  std::string FindNextWord(long int pos) const;

  /** Find the closing bracket given the position of the opening bracket. */
  long int FindClosingChar(char openChar, char closeChar, long int pos,bool noComment=false) const;

 
  /** Find the constructor in the file. */
  long FindConstructor(const std::string & buffer, const std::string & className, bool headerfile=true, size_t startPos=0) const;

  /** Find first occurrence of a SetFunction which is not implemented by a macro. 
      Return position after function ")" if found or any position after "void" if not found.
      flag = true indicates successful finding, otherwise false.
      isVoidSetFunc is returned as true if a function like "void Set...()" or
      "void Set...(void)" is detected.
   */
  size_t FindAndAddNonMacroSetFunction(std::string buffer, long startPos,
                                       std::string &paramName, 
                                       std::string &paramType,
                                       bool &flag,
                                       bool &isVoidSetFunc) const;

  /** Starting from startPos scan for all SetFunctions which are not implemented 
    by a macro. Add their type and name to the list of parameters. */
  //void FindAndAddNonMacroSetFunctions(std::string buffer, XMLDescription &desc, long startPos) const;

  /** Find the defaults var values in the .txx or .cxx file. */
  //void FindAndAddDefaultValues(std::string buffer, long start, XMLDescription &desc) const;

  /** Return true if the position pos is between <>.
   *  The Fast version just check for <test> and not for <test<>,test<>>*/
  bool IsBetweenCharsFast(const char begin, const char end, long int pos,bool withComments=false) const;
  bool IsBetweenChars(const char begin, const char end, long int pos,bool withComments=false) const;

  /** Removes ass CtrlN characters from the buffer. */
  void RemoveCtrlN(std::string & buffer) const;

  /** Find public area in source code. */
  void FindPublicArea(long &before, long &after, size_t startPos=0) const;

  /** Find protected area in source code. */
  void FindProtectedArea(long &before, long &after, size_t startPos=0) const;

  /** Find private area in source code. */
  void FindPrivateArea(long &before, long &after, size_t startPos=0) const;

  /** Find the maximum setInput method. In some classes it helps to detect
      the correct number of inputs. */
  int FindSetInputNum(const std::string & buffer, size_t startPos=0) const;

  /** Return the position of the template if the class has a template otherwise -1. */
  long int IsTemplated(const std::string & buffer, long int pos) const;

  /** Find the comments. */
  //bool FindAndAddComments(const std::string & buffer, XMLDescription &desc) const;

  /** Remove string area from start to end (incl) from buffer if filename contains className. */
  /*std::string RemoveArea(const std::string fileName,
                         std::string buffer, 
                         const std::string className,
                         const std::string &start, const std::string &end);
*/
  /**  return true if the position pos is inside a comment */
  bool Parser::IsInComments(long int pos) const;

  /**  return true if the position pos is inside a comment defined as the compiler
   *   i.e // or /* */
  bool Parser::IsInAnyComments(long int pos) const;

  /** Given the position without comments return the position with the comments */
  long int GetPositionWithComments(long int pos) const;

  /** Init the indentation step for CheckIndent() */
  bool Parser::InitIndentation();

  /** Extract the current line from pos to  \n */
  std::string ExtractLine(long pos);

  /** Return the current ident */
  long int GetCurrentIdent(std::string line,char type);

  /** Helper function to add words to the special indent vector */
  void AddIndent(const char* name,long int current,long int after);

private:

  ErrorVectorType m_ErrorList;
  std::vector<Info> m_InfoList;
  bool m_TestsDone[NUMBER_ERRORS];
  std::string m_TestsDescription[NUMBER_ERRORS];

  std::string m_Buffer;
  std::string m_BufferNoComment;
  std::vector<long int> m_Positions;
  typedef std::pair<long int, long int> PairType;
  std::vector<PairType> m_CommentPositions;
  std::string m_Filename;
  std::string m_HeaderFilename;
  std::string m_CommentBegin;
  std::string m_CommentMiddle;
  std::string m_CommentEnd;

  std::vector<IndentPosition> m_IdentPositionVector;

  std::string m_BlackList;
  std::string m_BlackListBuffer;
};

} // end namespace kws

#endif
