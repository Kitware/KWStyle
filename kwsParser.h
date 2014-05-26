/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsParser.h

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __kwsParser_h
#define __kwsParser_h

// Put kwssys/Configure.h first for proper compiler warning suppression:
#include <kwssys/Configure.h>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <kwssys/SystemTools.hxx>
#include <kwssys/Directory.hxx>
#include <kwssys/RegularExpression.hxx>
#include <algorithm>

namespace kws
{
#define MAX_CHAR 99999999
#define NUMBER_ERRORS 34

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
  SVAR_REGEX,
  SVAR_ALIGN,
  VARS,
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
  BLACKLIST,
  STATEMENTPERLINE,
  VARIABLEPERLINE,
  BADCHARACTERS,
  MEMBERFUNCTION_REGEX,
  MEMBERFUNCTION_LENGTH,
  FUNCTION_REGEX,
  FUNCTION_LENGTH,
  USING_DIRECTIVES,
  RELATIVE_PATH_IN_INCLUDE
  } ErrorType;

const char ErrorTag[NUMBER_ERRORS][4] = {
   {'L','E','N','\0'},
   {'I','V','P','\0'},
   {'I','V','R','\0'},
   {'I','V','A','\0'},
   {'S','V','R','\0'},
   {'S','V','A','\0'},
   {'V','A','R','\0'},
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
   {'B','L','K','\0'},
   {'S','P','L','\0'},
   {'V','P','L','\0'},
   {'B','C','H','\0'},   
   {'M','B','F','\0'},
   {'M','F','L','\0'},
   {'F','R','G','\0'},
   {'F','L','N','\0'},
   {'U','N','D','\0'},
   {'R','P','I','\0'}
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
  unsigned long line; // main line of the warning
  unsigned long line2; // second line of warning if the warning is covering several lines
  unsigned long number;
  std::string description; 
  } Warning;

typedef struct
  {
  // Position in the file
  size_t position;
  // What should be the position in the line of
  // the word w.r.t the previous position 
  int current;
  // What should be the position in the line of
  // the words after w.r.t the previous position 
  int after;
  // Name of the current ident
  std::string name;
  } IndentPosition;

class Parser
{
public:

  /** Constructor */
  Parser();

  /** Destructor */
  ~Parser();

  /** To be able to use std::sort we provide the < operator */
  bool operator<(const Parser& a) const;
  
  typedef std::vector<Error>                ErrorVectorType;
  typedef std::vector<Warning>              WarningVectorType;
  typedef std::pair<size_t,size_t>          IfElseEndifPairType;
  typedef std::vector<IfElseEndifPairType>  IfElseEndifListType;

  /** Set the buffer to analyze */
  void SetBuffer(std::string buffer) 
    {
    m_Buffer = buffer;
    this->ConvertBufferToWindowsFileType(m_Buffer);
    m_FixedBuffer = m_Buffer;
    this->RemoveComments();
    }
 
  /** Return the error tag as string given the error number */
  std::string GetErrorTag(unsigned long number) const;

  /** Return the error vector */
  const ErrorVectorType & GetErrors() const {return m_ErrorList;}

  /** Return the last error message */
  std::string GetLastErrors();

  /** Return the warning vector */
  std::string GetLastWarnings();

  /** Return the warning vector */
  const WarningVectorType & GetWarnings() const {return m_WarningList;}

  /** Check if the file contains "using namespace .* ;" */
  bool CheckUsingDirectives(bool forbidUsingDirectives);

  /** Check if the file contains includes with paths that refer to parent directory (ie: #include "../folder/file.h" ) */
  bool CheckRelativePathInInclude(bool forbidRelativePaths);

  /** Check if the file contains tabs */
  bool CheckTabs();

  /** Check the number of succesive empty lines */
  bool CheckEmptyLines(unsigned long max,bool checkEndOfFile=false);

  /** Check the comments
   * The comment definition should be set before CheckIndent() to get the correct indentation
   * for the comments. */
  bool CheckComments(const char* begin,const char* middle,const char* end,
                     bool allowEmptyLineBeforeClass=false,
                     bool checkWrongComment=true,
                     bool checkMissingComment=true);

  /** Check the indent size 
   *  Not in the header file if there is one 
   *  If CheckHeader has been done before CheckIndent and doNotCheckHeader is set to true
   *  then the header is not checked for indent*/
  bool CheckIndent(IndentType,
                   unsigned long size,
                   bool doNotCheckHeader=false,
                   bool allowBlockLine = false,
                   unsigned int maxLength = 81,
                   bool allowCommaIndent = true
                   );

  /** Check Operator spaces foo=bar or foo = bar, etc... */
  bool CheckOperator(unsigned int foo, unsigned int bar,
                     unsigned long maxSize=81,
                     bool doNotCheckInsideParenthesis=true);

  /** Check the number of character per line */
  bool CheckLineLength(unsigned long max,bool checkHeader=false);

  /** Check if the internal parameters of the class are correct */
  bool CheckInternalVariables(const char* regEx,bool alignement = true,bool checkProtected=false);
  
  /** Check variables implementation */
  bool CheckVariables(const char* regEx);
  
  /** Check Member Functions implementation */
  bool CheckMemberFunctions(const char* regEx,unsigned long maxLength=0);

  /** Check any Functions implementation */
  bool CheckFunctions(const char* regEx,unsigned long maxLength=0);

  /** Check if the strcut parameters of the class are correct */
  bool CheckStruct(const char* regEx,bool alignement = true);

  /** Check if the typedefs of the class are correct */
  bool CheckTypedefs(const char* regEx, bool alignment = true,
                     unsigned int maxLength = 81);

  /** Check the order of the declaration */
  bool CheckDeclarationOrder(size_t posPublic, size_t posProtected,size_t posPrivate);

  /** Check for extra spaces */
  bool CheckExtraSpaces(unsigned long max,bool checkEmptyLines=false);

  /** Check the number of space between the end of the declaration
   *  and the semicolon */
  bool CheckSemicolonSpace(unsigned long max);
 
  /** Check the number of statements per line */
  bool CheckStatementPerLine(unsigned long max=1,
                             bool checkInlineFunctions=true);
  
  /** Check the number of variables per line */
  bool CheckVariablePerLine(unsigned long max=1);

  /** Check bad characters */
  bool CheckBadCharacters(bool withComments=true);

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
  std::string RemoveComments(const char* buffer=NULL);

  /** Clear the error list */
  void ClearErrors() {m_ErrorList.clear();}
  
  /** Clear the info list */
  void ClearInfo() {m_WarningList.clear();}

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
  
  /** Should KWStyle produce a fix version of the parsed file */
  void SetFixFile(bool fix) {m_FixFile = fix;}
  void GenerateFixedFile();

  /**  return true if the position pos is between " " */
  bool IsBetweenQuote(size_t pos,bool withComments=false,std::string buffer="") const;

protected:

  /** Convert the file with \r\n instead of \n */ 
  void ConvertBufferToWindowsFileType(std::string & buffer);
  
  /** Check the operator.
   *  \warning This function add an error in the Error list */
  bool FindOperator(const char* op,unsigned int before, 
                    unsigned int after,unsigned long maxSize,
                    bool doNotCheckInsideParenthesis=true);

  /** Get the class position within the file. This function checks that this is the 
   *  classname */
  size_t GetClassPosition(size_t position,std::string buffer="") const;

  /** Return the position in the line given the position in the text */ 
  size_t GetPositionInLine(size_t pos);

  /** Find an ivar in the source code */
  std::string FindInternalVariable(size_t start, size_t end,size_t& pos);
  
  /** Find an ivar in the source code */
  std::string FindVariable(std::string & buffer,size_t start, size_t end,size_t& pos);
  
  /** Find a member function in the source code */
  std::string FindMemberFunction(std::string & buffer,size_t start, size_t end,size_t& pos);
 
  /** Find a typedef in the source code */
  std::string FindTypedef(size_t start, size_t end,size_t& pos,size_t & beg,size_t & typedefpos);

  /** Reduces multiple spaces in buffer to one. */
  void ReduceMultipleSpaces(std::string & buffer);

  /** Removes all val chars from string. */
  void RemoveChar(std::string & buffer, char val) const;

  /** Find the line number in the commented text given the character description */
  long int GetLineNumber(size_t pos,bool withoutComments=false) const;

  /** Find the previous word given a position */
  std::string FindPreviousWord(size_t pos,bool withComments=false,std::string buffer="") const;

  /** Find the next word given a position. This function works only without comments.*/
  std::string FindNextWord(size_t pos) const;

  /** Find the closing bracket given the position of the opening bracket. */
  size_t FindClosingChar(char openChar, char closeChar, size_t pos,bool noComment=false,std::string buffer="") const;

  /** Find the opening bracket given the position of the closing bracket. */
  size_t FindOpeningChar(char closeChar, char openChar, size_t pos,bool noComment=false) const;

  /** Find the constructor in the file. */
  size_t FindConstructor(const std::string & buffer, const std::string & className, 
                         bool headerfile=true, size_t startPos=0) const;

  /** Return true if the position pos is between <>.
   *  The Fast version just check for <test> and not for <test<>,test<>>*/
  bool IsBetweenCharsFast(const char begin, const char end, size_t pos,bool withComments=false,std::string buffer="") const;
  bool IsBetweenChars(const char begin, const char end, size_t pos,bool withComments=false,std::string buffer="") const;

  bool IsValidQuote(std::string & stream,size_t pos) const;

  /** Removes ass CtrlN characters from the buffer. */
  void RemoveCtrlN(std::string & buffer) const;

  /** Find the correct area given its name */
  size_t FindArea(const char* name,size_t startPos) const;

  /** Find public area in source code. */
  void FindPublicArea(size_t &before, size_t &after, size_t startPos=0) const;

  /** Find protected area in source code. */
  void FindProtectedArea(size_t &before, size_t &after, size_t startPos=0) const;

  /** Find private area in source code. */
  void FindPrivateArea(size_t &before, size_t &after, size_t startPos=0) const;

  /** Return the position of the template if the class has a template otherwise npos. */
  size_t IsTemplated(const std::string & buffer, size_t pos) const;

  /**  Return true if the position pos is inside a comment */
  bool IsInComments(size_t pos) const;
  
  /**  Return true if the position pos is inside a function */
  bool IsInFunction(size_t pos,const char* buffer=NULL) const;

  /**  Return true if the position pos is inside a struct */
  bool IsInStruct(size_t pos,const char* buffer=NULL) const;
  
  /**  Return true if the position pos is inside a union */
  bool IsInUnion(size_t pos,const char* buffer=NULL) const;

  /**  Return true if the position pos is inside a comment defined by the compiler */
  bool IsInAnyComments(size_t pos) const;

  /** Given the position without comments return the position with the comments */
  size_t GetPositionWithComments(size_t pos) const;
  size_t GetPositionWithoutComments(size_t pos) const;

  /** Init the indentation step for CheckIndent() */
  bool InitIndentation();

  /** Extract the current line from pos to  \n */
  std::string ExtractLine(size_t pos);

  /** Return the current ident */
  int GetCurrentIdent(std::string line,char type);

  /** Helper function to add words to the special indent vector */
  void AddIndent(const char* name,int current,int after);

  /** Find the end of the class */
  size_t FindEndOfClass(size_t position) const;

  /** Return if the dept of the current class */
  int IsInClass(size_t position) const;

  /** Return the position of the last character 
   *  of the function name/definition/ */
  size_t FindFunction(size_t position,const char* buffer=NULL) const;

  /** Compute the list of #if/#else/#endif */
  void ComputeIfElseEndifList();
  bool IsInElseForbiddenSection(size_t pos);

  /** Functions to deal with the fixed buffer */
  void ReplaceCharInFixedBuffer(size_t pos,size_t size,const char* replacingString);

  /** Check if the current position is a valid switch statement */
  bool CheckValidSwitchStatement(unsigned int posSwitch);
            
private:

  ErrorVectorType   m_ErrorList;
  WarningVectorType m_WarningList;
  bool m_TestsDone[NUMBER_ERRORS];
  std::string m_TestsDescription[NUMBER_ERRORS];

  std::string m_Buffer;
  std::string m_BufferNoComment;
  std::string m_FixedBuffer;
  std::vector<size_t> m_Positions;
  typedef std::pair<size_t, size_t> PairType;
  std::vector<PairType> m_CommentPositions;
  std::string m_Filename;
  std::string m_HeaderFilename;
  std::string m_CommentBegin;
  std::string m_CommentMiddle;
  std::string m_CommentEnd;
  bool        m_FixFile;
  std::vector<PairType> m_FixedPositions;

  std::vector<IndentPosition> m_IdentPositionVector;

  std::string m_BlackList;
  std::string m_BlackListBuffer;

  IfElseEndifListType m_IfElseEndifList;
};

} // end namespace kws

#endif
