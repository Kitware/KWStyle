/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsParser.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

#include <stdlib.h> // atoi
#include <string.h>
#include <kwssys/ios/sstream>
#include <boost/xpressive/xpressive.hpp>

using namespace boost::xpressive;

namespace kws {

/** Constructor */
Parser::Parser()
{
  m_HeaderFilename = "";
  m_Filename = "";
  m_FixFile = false;
  m_FixedPositions.clear();

  for(unsigned int i=0;i<NUMBER_ERRORS;i++)
    {
    m_TestsDone[i] = false;
    m_TestsDescription[i] += "NA";
    }

}

/** Destructor */
Parser::~Parser()
{
}

 /** To be able to use std::sort we provide the < operator */
bool Parser::operator<(const Parser& a) const
{
  if(m_Filename.compare(a.m_Filename))
    {
    return false;
    }
   return true;
}
  
/** Given the name of the check to perform and the default value perform the check */
bool Parser::Check(const char* name, const char* value)
{
  if(!strcmp(name,"LineLength"))
    {
    this->CheckLineLength(atoi(value));
    return true;
    }
  else if(!strcmp(name,"DeclarationOrder"))
    {
    this->CheckDeclarationOrder(atoi(&value[0]),atoi(&value[2]),atoi(&value[4]));
    return true;
    }
  else if(!strcmp(name,"Typedefs"))
    {
    bool alignment = true; // check alignment by default
    std::string val = value;
    long pos = val.find(",",0);
    if(pos != -1)
      {
      std::string v1 = val.substr(0,pos);

      // Check the alignment
      long pos1 = val.find(",",pos+1);

      std::string v2 = "";
      if(pos1 == -1)
        {
        v2 = val.substr(pos+1,val.size()-pos-1);
        }
      else
        {
        v2 = val.substr(pos+1,pos1-pos-1);
        }

      if(!strcmp(v2.c_str(),"false") || !strcmp(v2.c_str(),"0"))
        {
        alignment = false;
        }
      val = v1;
      }
    
    this->CheckTypedefs(val.c_str(),alignment);
    return true;
    }
  else if(!strcmp(name,"InternalVariables"))
    {
    bool alignment = true; // check alignment by default
    bool checkProtected = false; // check protected by default
    std::string val = value;
    long pos = val.find(",",0);
    if(pos != -1)
      {
      std::string v1 = val.substr(0,pos);

      // Check the alignment
      long pos1 = val.find(",",pos+1);

      std::string v2 = "";
      if(pos1 == -1)
        {
        v2 = val.substr(pos+1,val.size()-pos-1);
        }
      else
        {
        v2 = val.substr(pos+1,pos1-pos-1);
        }

      if(!strcmp(v2.c_str(),"false") || !strcmp(v2.c_str(),"0"))
        {
        alignment = false;
        }

      if(pos1 != -1)
        {
        // Check the protected
        pos = val.find(",",pos1+1);
        std::string v3 = "";
        if(pos == -1)
          {
          v3 = val.substr(pos1+1,val.size()-pos1-1);
          }
        else
          {
          v3 = val.substr(pos1+1,pos-pos1-1);
          }

        if(!strcmp(v3.c_str(),"true") || !strcmp(v3.c_str(),"1"))
          {
          checkProtected = true;
          }
        }
      val = v1;
      }
    
    this->CheckInternalVariables(val.c_str(),alignment,checkProtected);

    return true;
    }
  else if(!strcmp(name,"Variables"))
    {
    this->CheckVariables(value);
    return true;
    }
  else if(!strcmp(name,"Struct"))
    {
    this->CheckStruct(value);
    return true;
    }
  else if(!strcmp(name,"MemberFunctions"))
    {
    std::string val = value;
    std::string v1 = value;
    std::string v2 = "0";
    long pos = val.find(",",0);
    if(pos != -1)
      {
      v1 = val.substr(0,pos);
      v2 = val.substr(pos+1,val.size()-pos-1);
      }
    this->CheckMemberFunctions(v1.c_str(),atoi(v2.c_str()));
    return true;
    }
  else if(!strcmp(name,"Functions"))
    {
    std::string val = value;
    std::string v1 = value;
    std::string v2 = "0";
    long pos = val.find(",",0);
    if(pos != -1)
      {
      v1 = val.substr(0,pos);
      v2 = val.substr(pos+1,val.size()-pos-1);
      }
    this->CheckFunctions(v1.c_str(),atoi(v2.c_str()));
    return true;
    }
  else if(!strcmp(name,"SemicolonSpace"))
    {
    this->CheckSemicolonSpace(atoi(value));
    return true;
    }
  else if(!strcmp(name,"EndOfFileNewLine"))
    {
    this->CheckEndOfFileNewLine();
    return true;
    }
  else if(!strcmp(name,"Tabs"))
    {
    this->CheckTabs();
    return true;
    }
  else if(!strcmp(name,"Spaces"))
    {
    this->CheckExtraSpaces(atoi(value));
    return true;
    }
  else if(!strcmp(name,"StatementPerLine"))
    {
    std::string val = value;
    std::string v1 = value;
    long pos = val.find(",",0);
    bool checkInlineFunctions = true;
    if(pos != -1)
      {
      v1 = val.substr(0,pos);
      std::string v2 = val.substr(pos+1,val.size()-pos-1);
      if(v2 == "0")
        {
        checkInlineFunctions = false;
        }
      }

    this->CheckStatementPerLine(atoi(v1.c_str()),checkInlineFunctions);
    return true;
    }
 else if(!strcmp(name,"BadCharacters"))
    {
    if(!strcmp(value,"true"))
      {
      this->CheckBadCharacters(true);
      }
    else
      {
      this->CheckBadCharacters(false);
      }
    return true;
    }
  else if(!strcmp(name,"VariablePerLine"))
    {
    this->CheckVariablePerLine(atoi(value));
    return true;
    }
  else if(!strcmp(name,"Comments"))
    {
    std::string val = value;
    long pos = val.find(",",0);
    if(pos == -1)
      {
      std::cout << "Comments not defined correctly" << std::endl;
      return false;
      }
    std::string v1 = val.substr(0,pos);
    long int pos1 = val.find(",",pos+1);
    if(pos1 == -1)
      {
      std::cout << "Comments not defined correctly" << std::endl;
      return false;
      }
    std::string v2 = val.substr(pos+1,pos1-pos-1);
    pos = val.find(",",pos1+1);
    if(pos == -1)
      {
      std::cout << "Comments not defined correctly" << std::endl;
      return false;
      }
    std::string v3 = val.substr(pos1+1,pos-pos1-1);

    // Check if we allow empty lines before /class
    pos1 = val.find(",",pos+1);
    std::string v4 = "";
    if(pos1 == -1)
      {
      v4 = val.substr(pos+1,val.length()-pos-1);
      }
    else
      {
      v4 = val.substr(pos+1,pos1-pos-1);
      }

    bool allowEmptyLine = false;
    if(!strcmp(v4.c_str(),"true"))
      {
      allowEmptyLine = true;
      }

    // Check if we should check the comments misspeling
    pos = val.find(",",pos1+1);
    std::string v5 = "";
    if(pos == -1)
      {
      v5 = val.substr(pos1+1,val.length()-pos1-1);
      }
    else
      {
      v5 = val.substr(pos1+1,pos-pos1-1);
      }

    bool checkWrongComment = true;
    if(!strcmp(v5.c_str(),"false"))
      {
      checkWrongComment = false;
      }

    // Check if we should check the missing comments
    pos1 = val.find(",",pos+1);
    std::string v6 = "";
    if(pos == -1)
      {
      v6 = val.substr(pos+1,val.length()-pos-1);
      }
    else
      {
      v6 = val.substr(pos+1,pos1-pos-1);
      }

    bool checkMissingComment = true;
    if(!strcmp(v6.c_str(),"false"))
      {
      checkMissingComment = false;
      }

    this->CheckComments(v1.c_str(),v2.c_str(),v3.c_str(),allowEmptyLine,checkWrongComment,checkMissingComment);

    return true;
    }
  // should be before CheckIndent
  else if(!strcmp(name,"Header"))
    {
    std::string val = value;
    long pos = val.find(",",0);
    if(pos == -1)
      {
      std::cout << "Header not defined correctly" << std::endl;
      return false;
      }

    std::string v1 = "";
    if(pos>0)
      {
      v1 = val.substr(0,pos);
      }

    long int pos1 = val.find(",",pos+1);
    if(pos1 == -1)
      {
      std::cout << "Header not defined correctly" << std::endl;
      return false;
      }

    std::string v2 = val.substr(pos+1,pos1-pos-1);
    std::string v3 = val.substr(pos1+1,val.length()-pos1-1);

    bool spaceEndOfLine = false;
    bool useCVS = false;
 
    if(!strcmp(v2.c_str(),"true"))
      {
      spaceEndOfLine = true;
      }
    if(!strcmp(v3.c_str(),"true"))
      {
      useCVS = true;
      }

    if(v1.size()>0)
      {
      this->CheckHeader(v1.c_str(),spaceEndOfLine,useCVS);
      }
    }

   else if(!strcmp(name,"Indent"))
    {
    std::string val = value;
    long pos = val.find(",",0);
    if(pos == -1)
      {
      std::cout << "Indent not defined correctly" << std::endl;
      return false;
      }
    std::string v1 = val.substr(0,pos);
    long int pos1 = val.find(",",pos+1);
    if(pos1 == -1)
      {
      std::cout << "Indent not defined correctly" << std::endl;
      return false;
      }
    std::string v2 = val.substr(pos+1,pos1-pos-1);
    pos = val.find(",",pos1+1);
    if(pos == -1)
      {
      std::cout << "Indent not defined correctly" << std::endl;
      return false;
      }
    std::string v3 = val.substr(pos1+1,pos-pos1-1);
    std::string v4 = val.substr(pos+1,val.length()-pos-1);
    bool header = false;
    if(!strcmp(v3.c_str(),"true"))
      {
      header = true;
      }
    bool blockline = false;
    if(!strcmp(v4.c_str(),"true"))
      {
      blockline = true;
      }

    IndentType itype = kws::SPACE;

    if(!strcmp(v1.c_str(),"TAB"))
      {
      itype = kws::TAB;
      }
    this->CheckIndent(itype,atoi(v2.c_str()),header,blockline);
    }

  else if(!strcmp(name,"Namespace"))
    {
    this->CheckNamespace(value);
    }
  else if(!strcmp(name,"NameOfClass"))
    {
    std::string val = value;
    long pos = val.find(",",0);
    if(pos == -1)
      {
      std::cout << "NameOfClass not defined correctly" << std::endl;
      return false;
      }
    std::string v1 = val.substr(0,pos);
    std::string v2 = val.substr(pos+1,val.length()-pos-1);
    this->CheckNameOfClass(v1.c_str(),v2.c_str());
    }
  else if(!strcmp(name,"IfNDefDefine"))
    {
    std::string val = value;
    std::string v1 = value;
    bool uppercaseTheDefinition = false;
    long pos = val.find(",", 0);
    if(pos != -1)
      {
      v1 = val.substr(0, pos);
      std::string v2 = val.substr(pos+1);
      uppercaseTheDefinition = !v2.compare("true") || !v2.compare("1");
      }
    this->CheckIfNDefDefine(v1.c_str(), uppercaseTheDefinition);
    }
  else if(!strcmp(name,"EmptyLines"))
    {
    this->CheckEmptyLines(atoi(value));
    }
  else if(!strcmp(name,"Template"))
    {
    this->CheckTemplate(value);
    }
  else if(!strcmp(name,"Operator"))
    {
    std::string val = value;
    long pos = val.find(",",0);
    if(pos == -1)
      {
      std::cout << "Operator not defined correctly" << std::endl;
      return false;
      }
    std::string v1 = val.substr(0,pos);
    std::string v2 = val.substr(pos+1,val.length()-pos-1);
    this->CheckOperator(atoi(v1.c_str()),atoi(v2.c_str()));
    }
  else if(!strcmp(name,"BlackList"))
    {
    this->CheckBlackList(value);
    }
  return false;
}


/** Return if a test has been performed */
bool Parser::HasBeenPerformed(unsigned int test) const
{
  return m_TestsDone[test];
}

/** Return the test description given the erro number) */
std::string Parser::GetTestDescription(unsigned int test) const
{
  return m_TestsDescription[test];
}


/** Return the last errors as a string */
std::string Parser::GetLastErrors()
{
  std::string output = "";
  std::vector<Error>::const_iterator it = m_ErrorList.begin();
  while(it != m_ErrorList.end())
    {
    output += "Error #";
    char* val = new char[10];
    sprintf(val,"%ld",(*it).number);
    output += val;
    delete [] val;
    output += " (";
    val = new char[10];
    sprintf(val,"%ld",(*it).line);
    output += val;
    delete [] val;
    output += ") ";
    output += (*it).description;
    output += "\n";
    it++;
    }
  return output;
}

/** Return the error tag as string given the error number */
std::string Parser::GetErrorTag(unsigned long number) const
{
  return ErrorTag[number];
}

/** Return the last warnings as a string */
std::string Parser::GetLastWarnings()
{
  std::string output = "";
  std::vector<Warning>::const_iterator it = m_WarningList.begin();
  while(it != m_WarningList.end())
    {
    output += "Warning #";
    char* val = new char[10];
    sprintf(val,"%ld",(*it).number);
    output += val;
    delete [] val;
    output += " (";
    val = new char[10];
    sprintf(val,"%ld",(*it).line);
    output += val;
    delete [] val;
    output += ") ";
    output += (*it).description;
    output += "\n";
    it++;
    }
  return output;
}

void Parser::ConvertBufferToWindowsFileType(std::string & buffer)
{
  // replace logical newlines with windows newlines
  sregex unixNewline = sregex::compile("\r?\n|\r");
  std::string windowsNewline("\r\n");
  buffer = regex_replace(buffer, unixNewline, windowsNewline);
}

/** Return the number of lines */
unsigned long Parser::GetNumberOfLines() const
{
 unsigned long lines = 0;
 long int pos = m_Buffer.find("\n",0);
 while(pos != -1)
   {
   lines++;
   pos = m_Buffer.find("\n",pos+1);
   }
 lines++; // the last line doesn't have any \n
 return lines;
}

/** Return the line */
std::string Parser::GetLine(unsigned long i) const
{
 unsigned long lines = 0;
 long int prec = 0;
 long int pos = m_Buffer.find("\n",0);
 while(pos != -1)
   {
   if(lines == i)
     {
     if(pos-prec-1>0)
       {
       return m_Buffer.substr(prec,pos-prec-1);
       }
     }
   lines++;
   prec = pos;
   pos = m_Buffer.find("\n",pos+1);
   }
  
  if(lines == i)
    {
    return m_Buffer.substr(prec,m_Buffer.size()-prec);
    }
 
 return "";
}

/** Find the previous word given a position */
std::string Parser::FindPreviousWord(size_t pos,bool withComments,std::string buffer) const
{
  std::string stream = buffer;
  if(buffer.size() == 0)
    {
    stream = m_BufferNoComment;
    if(withComments)
      {
      stream = m_Buffer;
      }
    }

  size_t i=pos;

  while(stream[i] != ' ' && stream[i] != '\r' && i>0)
    {
    i--;
    }

  if(stream[i] == '\r' && i>1)
    {
    i--;
    }

  bool inWord = true;
  bool first = false;
  std::string ivar = "";
  while(i != std::string::npos && inWord)
    {
    if(stream[i] != ' ' && stream[i] != '\n' && stream[i] != '\r')
      {
      std::string store = ivar;
      ivar = stream[i];
      ivar += store;
      inWord = true;
      first = true;
      }
    else // we have a space
      {
      if(stream[i] == '\r')
        {
        // do nothing
        }
      else if(first)
        {
        inWord = false;
        }
      }
    i--;
    }
  return ivar;
}

/** Find the next word given a position */
std::string Parser::FindNextWord(size_t pos) const
{
  size_t i=pos;

  // we go to the next space
  while(m_BufferNoComment[i] != ' ' && i<m_BufferNoComment.size())
    {
    i++;
    }

  bool inWord = true;
  bool first = false;
  std::string ivar = "";
  while(i<m_BufferNoComment.size() && inWord)
    {
    if(m_BufferNoComment[i] != ' ' && m_BufferNoComment[i] != '\r')
      {
      ivar += m_BufferNoComment[i];
      inWord = true;
      first = true;
      }
    else // we have a space
      {
      if(first)
        {
        inWord = false;
        }
      }
    i++;
    }
  return ivar;
}

/** Return the position in the line given the position in the text */ 
size_t Parser::GetPositionInLine(size_t pos)
{
  size_t begin = pos;
  while(begin != std::string::npos && m_BufferNoComment[begin]!='\n')
    {
    begin--;
    }
  return pos-begin;
}

/** Given the position without comments return the position with the comments */
size_t Parser::GetPositionWithComments(size_t pos) const
{
  std::vector<PairType>::const_iterator it = m_CommentPositions.begin();
  while(it != m_CommentPositions.end())
    {
    if((pos>=(*it).first))
      {
      pos += ((*it).second-(*it).first);
      }
    else
      {
      break;
      }
    it++;
    }
  return pos;
}  

/** Given the position with comments return the position without the comments */
size_t Parser::GetPositionWithoutComments(size_t pos) const
{
  size_t pos2 = pos;
  std::vector<PairType>::const_iterator it = m_CommentPositions.begin();
  while(it != m_CommentPositions.end())
    {
    if((pos>=(*it).first))
      {
      pos2 -= ((*it).second-(*it).first);
      }
    else
      {
      break;
      }
    it++;
    }
  return pos2;
}

/** Return if the dept of the current class 
 *  This function works on m_BufferNoComments! */
int Parser::IsInClass(size_t position) const
{
  int inClass = 0;
  size_t classPos = this->GetClassPosition(0);
  while(classPos!=std::string::npos)
    {
    size_t i=classPos;
    while(i!=std::string::npos && i<m_BufferNoComment.size())
      {
      if(m_BufferNoComment[i] == '{')
        {
        break;
        }
      i++;
      }
    size_t classEnd = this->FindClosingChar('{','}',i+1,true);
   
    if(position>classPos && position<classEnd)
      {
      inClass++;
      }
    classPos = this->GetClassPosition(classPos+1);
    }

  return inClass;
}


/** Return the line number in the source code given the character position */
long int Parser::GetLineNumber(size_t pos,bool withoutComments) const
{
  
  // if we have comments we add them to the list
  if(withoutComments)
    {
    pos = this->GetPositionWithComments(pos);
    }

  unsigned int i=0;
  std::vector<size_t>::const_iterator it = m_Positions.begin();
  while(it != m_Positions.end())
    {
    if(pos<=(*it))
      {
      return i; 
      }
    i++;
    it++;
    }
  return 0;
}

/** Find public area in source code. */
void Parser::FindPublicArea(size_t &before, size_t &after, size_t startPos) const
{
  before = 0;
  after = 0;

  // First look if public is before protected and private
  size_t pub = this->FindArea("public", startPos);
  size_t priv = this->FindArea("private", startPos);
  size_t protect = this->FindArea("protected", startPos);
  
  if(pub == std::string::npos)
    {
    // ok we don't have any public area
    before = std::string::npos;
    after = std::string::npos;
    return;
    pub = MAX_CHAR;
    }

  if(priv == std::string::npos)
    {
    priv = MAX_CHAR;
    }

  if(protect == std::string::npos)
    {
    protect = MAX_CHAR;
    }

  if(pub>priv || pub>protect)
     {
     if(pub>=priv && pub<=protect)
       {
       before = priv;
       after = protect;
       }
     else if(pub<=priv && pub>=protect)
       {
       before = pub;
       after = priv;
       }
     else
       {
       before = pub;
       size_t eoc = this->FindEndOfClass(pub);
       if(eoc != std::string::npos)
         {
         after = eoc;
         }
       else
         {
         after = pub; // end of class
         }
       }
     }
  else
    {
    before = pub;
  
    if(priv<protect)
      {
      after = priv;
      }
    else
      {
      after = protect;
      }
    }
  
  // If there is nothing after we point to the end of the class
  if(after == MAX_CHAR)
    {
    size_t classpos = this->GetClassPosition(startPos);
    if(classpos != std::string::npos)
      {
      size_t posBrace = m_BufferNoComment.find("{",classpos);          
      if(posBrace != std::string::npos)
        {
        size_t end = this->FindClosingChar('{','}',posBrace,true);
        if(end != std::string::npos)
          {
          after = end;
          }
        }
      }
    }
}
 
/** Find the correct area given its name 
 *  This looks for public: or public    : */
size_t Parser::FindArea(const char* name,size_t startPos) const
{
  size_t pos = m_BufferNoComment.find(name, startPos);
  while(pos != std::string::npos)
    {
    if(m_BufferNoComment[pos+strlen(name)]==':' || this->FindNextWord(pos) == ":")
      {
      return pos;
      }
    pos = m_BufferNoComment.find(name, pos+1);
    }
  return std::string::npos;
}


/** Find protected area in source code. */
void Parser::FindProtectedArea(size_t &before, size_t &after, size_t startPos) const
{
  before = 0;
  after = 0;

  // First look if public is before protected and private
  size_t pub = this->FindArea("public", startPos);
  size_t priv = this->FindArea("private", startPos);
  size_t protect = this->FindArea("protected", startPos);

  if(priv == std::string::npos)
    {
    priv = MAX_CHAR;
    }
  
  if(protect == std::string::npos)
    {
    protect = MAX_CHAR;
    }

  if(pub == std::string::npos)
    {
    pub = MAX_CHAR;
    }

  if(protect>priv || protect>pub)
     {
     if(protect>=priv && protect<=pub)
       {
       before = protect;
       after = pub;
       }
     else if(protect<=priv && protect>=pub)
       {
       before = protect;
       after = priv;
       }
     else
       {
       before = protect;
       size_t eoc = this->FindEndOfClass(protect);
       if(eoc != std::string::npos)
         {
         after = eoc;
         }
       else
         {
         after = protect; // end of class
         }
       }
     }
  else
    {
    before = protect;
    if(priv<pub)
      {
      after = priv;
      }
    else
      {
      after = pub;
      }
    }


  // If there is nothing after we point to the end of the class
  if(after == MAX_CHAR)
    {
    size_t classpos = this->GetClassPosition(startPos);
    if(classpos != std::string::npos)
      {
      size_t posBrace = m_BufferNoComment.find("{",classpos);          
      if(posBrace != std::string::npos)
        {
        size_t end = this->FindClosingChar('{','}',posBrace,true);
        if(end != std::string::npos)
          {
          after = end;
          }
        }
      }
    }
}

/** Find the end of the class */
size_t Parser::FindEndOfClass(size_t position) const
{
  if(position == std::string::npos)
    {
    position = 0;
    }

  // Try to find the end of the class
  size_t endclass = m_BufferNoComment.find("}",position);
  while(endclass != std::string::npos)
    {
    bool isClass = true;
    // if the next char is not a semicolon this cannot be a class
    for(size_t i=endclass+1;i<m_BufferNoComment.size();i++)
      {
      if( (m_BufferNoComment[i] != ' ') && (m_BufferNoComment[i] != '\r')
        && (m_BufferNoComment[i] != '\n') && (m_BufferNoComment[i] != ';')
        )
        {
        isClass = false;
        break;
        }
      if(m_BufferNoComment[i] == ';')
        {
        break;
        }
      }

    if(isClass)
      {
      size_t openingChar = this->FindOpeningChar('}','{',endclass,true);
      // check if we have the class name somewhere
      size_t classPos = m_BufferNoComment.find("class",0);
      while(classPos != std::string::npos && classPos<position)
        {
        if(classPos != std::string::npos && openingChar!= std::string::npos)
          {
          for(size_t i=classPos;i<openingChar+1;i++)
            {
            if(m_BufferNoComment[i] == '{')
              {
              classPos = i;
              break;
              }
            }
          if(openingChar == classPos)
            {
            return endclass;
            }
          }
        classPos = m_BufferNoComment.find("class",classPos+1);
        }
      }     
    endclass = m_BufferNoComment.find("}",endclass+1);
    }
  return std::string::npos;
}


/** Find private area in source code. */
void Parser::FindPrivateArea(size_t &before, size_t &after, size_t startPos) const
{
  before = 0;
  after = 0;

  // First look if public is before protected and private
  size_t pub = this->FindArea("public", startPos);
  size_t priv = this->FindArea("private", startPos);
  size_t protect = this->FindArea("protected", startPos);
  
  if(priv == std::string::npos)
    {
    priv = MAX_CHAR;
    }

  if(pub == std::string::npos)
    {
    pub = MAX_CHAR;
    }

  if(protect == std::string::npos)
    {
    protect = MAX_CHAR;
    }

  if(priv>pub || priv>protect)
     {
     if(priv>=pub && priv<=protect)
       {
       before = priv;
       after = protect;
       }
     else if(priv<=pub && priv>=protect)
       {
       before = priv;
       after = pub;
       }
     else
       {
       before = priv;
       size_t eoc = this->FindEndOfClass(priv);
       if(eoc != std::string::npos)
         {
         after = eoc;
         }
       else
         {
         after = priv; // end of class
         }
       }
     }
  else
    {
    before = priv;
    if(pub<protect)
      {
      after = pub;
      }
    else
      {
      after = protect;
      }
    }

  // If there is nothing after we point to the end of the class
  if(after == MAX_CHAR)
    {
    size_t classpos = this->GetClassPosition(startPos);
    if(classpos != std::string::npos)
      {
      size_t posBrace = m_BufferNoComment.find("{",classpos);          
      if(posBrace != std::string::npos)
        {
        size_t end = this->FindClosingChar('{','}',posBrace,true);
        if(end != std::string::npos)
          {
          after = end;
          }
        }
      }
    }
}

/** Return true if the position pos is inside a function 
 *  This function works on the m_BufferNoComment */
bool Parser::IsInFunction(size_t pos,const char* buffer) const
{ 
  std::string buf = m_BufferNoComment;
  
  if(buffer)
    {
    buf = buffer;
    }

  if((pos == std::string::npos) || pos > buf.size()-1)
    {
    return false;
    }

  // a function is defined as:
  // function() {}; and if() {} is considered as a function
  // here.
  size_t close = 1;
  bool check = false;

  // We go backwards
  size_t i = pos;
  while(i != std::string::npos)
    {
    if(buf[i] == '}')
      {
      close++;
      }
    else if(buf[i] == '{')
      {
      check = true;
      close--;
      }
    else if(buf[i] == ')')
      {
      if(check && close==0)
        {
        return true;
        }
      }
    else if(buf[i] == ' '
            || buf[i] == '\r'
            || buf[i] == '\n'
            )
      {

      }
    else
      {
      //check = false;
      }
    i--;
    }

  return false;
}


/** Return the position of the last character 
 *  of the function name/definition */
size_t Parser::FindFunction(size_t pos,const char* buffer) const
{ 
  std::string buf = m_BufferNoComment;
  
  if(buffer)
    {
    buf = buffer;
    }

  if((pos == std::string::npos) || pos > buf.size()-1)
    {
    return std::string::npos;
    }

  // a function is defined as:
  // function() [const] {}; and if() {} is considered as a function
  // here.

  // We go backwards
  size_t end = buf.find('}',pos);        
  while(end != std::string::npos)
    {
    size_t beg = this->FindOpeningChar('}','{',end,true);

    // check that before the beg we have
    bool nospecialchar=true;
    size_t i = beg-1;
    while(i != std::string::npos)
      {
      if(buf[i] == ')')
        {
        break;
        }
      else if(
        buf[i] != ' ' && buf[i] != '\r'
        && buf[i] != '\n'
        )
        {
        nospecialchar = false;
        }
      i--;
      }

    if(nospecialchar)
      {
      return beg;
      }
    else if(i != std::string::npos)// check if we have a const
      {
      if(buf.substr(i,beg-i).find("]") == std::string::npos &&
         buf.substr(i,beg-i).find("const") != std::string::npos
        )
        {
        return beg;
        }
      }
    end = buf.find('}',end+1);
    }

  return std::string::npos;
}

/** Return true if the position pos is inside a struct 
 *  This function works on the m_BufferNoComment */
bool Parser::IsInStruct(size_t pos,const char* buffer) const
{ 
  std::string buf = m_BufferNoComment;
  if(buffer)
    {
    buf = buffer;
    }

  if((pos == std::string::npos) || pos > buf.size()-1)
    {
    return false;
    }
  
  size_t b = buf.find("struct",0);
  while(b!=std::string::npos)
    {
    // The next character should be either a space or a {
    if(b+6==buf.size())
      {
      return false;
      }
    
    if(buf[b+6]!=' ' && buf[b+6]!='{')
      {
      return false;
      }
    
    while(b<buf.size())
      {
      if(buf[b] == '{')
        {
        break;
        }
      b++;
      }

    size_t c=this->FindClosingChar('{','}',b,true);
    if(pos<c && pos>b)
      {
      return true;
      break;
      }
    b = buf.find("struct",b+1);
    }
  return false;
}


/** Return true if the position pos is inside a union 
 *  This function works on the m_BufferNoComment */
bool Parser::IsInUnion(size_t pos,const char* buffer) const
{ 
  std::string buf = m_BufferNoComment;
  if(buffer)
    {
    buf = buffer;
    }

  if((pos == std::string::npos) || pos > buf.size()-1)
    {
    return false;
    }
  
  size_t b = buf.find("union",0);
  while(b!=std::string::npos)
    {
    while(b<buf.size())
      {
      if(buf[b] == '{')
        {
        break;
        }
      b++;
      }

    size_t c=this->FindClosingChar('{','}',b,true);
    if(pos<c && pos>b)
      {
      return true;
      break;
      }
    b = buf.find("union",b+1);
    }
  return false;
}

/**  return true if the position pos is inside a comment */
bool Parser::IsInComments(size_t pos) const
{
  if((pos == std::string::npos) || (m_CommentBegin.size()==0) || (m_CommentEnd.size() == 0))
    {
    return false;
    }

  size_t b0 = m_Buffer.find(m_CommentBegin,0);
  
  if(b0 == std::string::npos)
   {
   return false;
   }

  size_t b1 = m_Buffer.find(m_CommentEnd,b0);
  
  while(b0 != std::string::npos && b1 != std::string::npos && b1>b0)
    {
    if(pos>=b0 && pos<=(b1+m_CommentEnd.size()))
      {
      return true;
      }
    b0 = m_Buffer.find(m_CommentBegin,b0+1);
    b1 = m_Buffer.find(m_CommentEnd,b0);
    }

  return false;
}

/** Return true is the position is in any comments */
bool Parser::IsInAnyComments(size_t pos) const
{
  //Check //
  size_t posslash = m_Buffer.find("//",0);
  size_t posend = std::string::npos;
  if(posslash != std::string::npos)
    {
    posend = m_Buffer.find("\n",posslash);
    }

  while(posend != std::string::npos)
    {
    if(pos<=posend && pos>=posslash)
      {
      return true;
      }
    posslash = m_Buffer.find("//",posend+1);
    if(posslash != std::string::npos)
      {
      posend = m_Buffer.find("\n",posslash);
      }
    else
      {
      posend = std::string::npos;
      }
    }

  // Check the /* */
  size_t posstart = m_Buffer.find("/*",0);
  posend = std::string::npos;
  if(posstart != std::string::npos)
    {
    posend = m_Buffer.find("*/",posstart);
    }

  while(posend != std::string::npos)
    {
    if(pos<=posend && pos>=posstart)
      {
      return true;
      }
    posstart = m_Buffer.find("/*",posend+1);
    if(posstart != std::string::npos)
      {
      posend = m_Buffer.find("*/",posstart);
      }
    else
      {
      posend = std::string::npos;
      }
    }
  return false;    
}

/** Return true if it's a valid quote */
bool Parser::IsValidQuote(std::string & stream,size_t pos) const
{
  // We need to count the number of \ if it's an odd number
  // then this is not a valid "
  int n = 0;
  size_t i = pos;
  if(pos != std::string::npos)
    {
    i = pos-1;
    }
  
  while(i != std::string::npos && stream[i]=='\\')
    {
    n++;
    i--;
    }

  if(n%2==0)
    {
    return true;
    }
  return false;
}

/**  return true if the position pos is between " " */
bool Parser::IsBetweenQuote(size_t pos,bool withComments,std::string buffer) const
{
  std::string stream = buffer;

  if(buffer.size()==0)
    {
    stream = m_BufferNoComment;
    if(withComments)
      {
      stream = m_Buffer;
      }
    }

  if(pos == std::string::npos)
    {
    return false;
    }

  // We don't want to check for \" otherwise it fails
  size_t b0 = stream.find('"',0);
  while((b0!=std::string::npos) && !this->IsValidQuote(stream,b0))
    {
    b0 = stream.find('"',b0+1);
    }

  size_t b1 = stream.find('"',b0+1);
  while((b1!=std::string::npos) && !this->IsValidQuote(stream,b1))
    {
    b1 = stream.find('"',b1+1);
    }

  while(b0 != std::string::npos && b1 != std::string::npos && b1>b0)
    {
    if(pos>b0 && pos<b1)
      {
      return true;
      }
    b0 = stream.find('"',b1+1);
    while((b0!=std::string::npos) && !this->IsValidQuote(stream,b0))
      {
      b0 = stream.find('"',b0+1);
      }

    b1 = stream.find('"',b0+1);
    while((b1!=std::string::npos) && !this->IsValidQuote(stream,b1))
      {
      b1 = stream.find('"',b1+1);
      }
    }
  return false;
}

/**  return true if the position pos is between 'begin' and 'end' */
bool Parser::IsBetweenCharsFast(const char begin, const char end ,size_t pos,bool withComments,std::string buffer) const
{
  std::string stream = buffer;
  if(buffer.size() == 0)
    {
    stream = m_BufferNoComment;
    if(withComments)
      {
      stream = m_Buffer;
      }
    }

  if(pos == std::string::npos)
    {
    return false;
    }

  size_t b0 = stream.find(begin,0);
  size_t b1 = stream.find(end,b0);

  while(b0 != std::string::npos && b1 != std::string::npos && b1>b0)
    {
    if(pos>b0 && pos<b1)
      {
      return true;
      }
    b0 = stream.find(begin,b0+1);
    b1 = stream.find(end,b0);
    }
  return false;
}

/**  return true if the position pos is between 'begin' and 'end' */
bool Parser::IsBetweenChars(const char begin, const char end ,size_t pos,
                            bool withComments,std::string buffer) const
{
  std::string stream = buffer;
  if(buffer.size() == 0)
    {
    stream = m_BufferNoComment;
    if(withComments)
      {
      stream = m_Buffer;
      }
    }
  
  if(pos == std::string::npos)
    {
    return false;
    }

  size_t b0 = stream.find(begin,0);
  size_t b1 = this->FindClosingChar(begin,end,b0,!withComments,buffer);

  while(b0 != std::string::npos && b1 != std::string::npos && b1>b0)
    {
    if(pos>b0 && pos<b1)
      {
      return true;
      }
    b0 = stream.find(begin,b0+1);
    b1 = this->FindClosingChar(begin,end,b0,!withComments,buffer);
    }
  return false;
}

/** Return true if the class has a template */
size_t Parser::IsTemplated(const std::string & buffer, size_t classnamepos) const
{
  size_t templatepos = buffer.find("template",0);
  size_t returnval = templatepos;

  while(templatepos!=std::string::npos && templatepos<classnamepos)
    {
    // find the last > that corresponding to the template
    size_t p0 = buffer.find("<",templatepos);
    size_t pos = std::string::npos;
    if(p0!=std::string::npos)
      {
      size_t i=p0+1;
      unsigned int n=1;

      while(i<classnamepos && n>0)
        {
        if(buffer[i] == '<')
          {
          n++;
          }
        else if(buffer[i] == '>')
          {
          n--;
          }
        if(n==0)
          {
          pos =i;
          break;
          }
        i++;
        }
      }

    if(pos<classnamepos && pos!=std::string::npos)
      {
      bool ok = true;
      // Check if between the last > and classnamepos we don't have words
      for(size_t i=pos+1;i<classnamepos;i++)
        {
        if(buffer.c_str()[i] != 13 && buffer.c_str()[i] != '\n' && buffer.c_str()[i]!= '\t')
          {
          ok = false;
          }
        }
      if(ok)
        {
        return templatepos;
        }
      /*else
        {
        std::cout << "ERROR in Parser::IsTemplated, Cannot find proper template parameters" << std::endl;
        system("PAUSE");
        }*/
      }
    returnval = templatepos;
    templatepos = buffer.find("template",templatepos+1);
    }

  return returnval;
}

/** Remove the comments */
std::string Parser::RemoveComments(const char* buffer)
{
  if(!buffer)
    {
    m_CommentPositions.clear();
    }
  size_t cc;
  const char* inch;
  if(buffer)
    {
    inch = buffer;
    }
  else
    {
    inch = m_Buffer.c_str();
    }

  std::vector<char> outBuffer;
  size_t inStrSize = m_Buffer.size();
  if(buffer)
    {
    std::string tempbuf = buffer;
    inStrSize = tempbuf.size();
    }
  // Reserve enough space for all files. The size will be at least the size of
  // the file.
  outBuffer.reserve(inStrSize+1);

  bool inString  = false;
  bool inComment = false;
  bool copyChar;
  bool cppComment = false;

  size_t beginOfComment = 0;
  size_t endOfComment = 0;

  for ( cc = 0; cc < inStrSize; ++ cc )
    {
    copyChar = true;

    // it is important to know if we are in a string---if we are then
    // we don't care about comment-ish symbols
    //
    // we are starting or ending a string if we find a " without a
    // preceeding '\'
    bool isStringBarrier = (*inch == '"') && (cc == 0 || *(inch-1) != '\\');
    inString = isStringBarrier ? !inString : inString;

    if (!inString)
      {
      if ( !inComment )
        {
        if ( *inch == '/' )
          {
          if ( cc < inStrSize-1 )
            {
            if ( *(inch+1) == '*' )
              {
              inComment = true;
              beginOfComment = cc;
              }
            else if ( *(inch+1) == '/' )
              {
              inComment = true;
              beginOfComment = cc;
              cppComment = true;
              }
            }
          }
        }
      else
        {
        if ( cppComment && *inch == '\n' )
          {
          endOfComment = cc;
          inComment = false;
          copyChar = true;
          cppComment = false;
          PairType pair(beginOfComment, endOfComment);
          if(!buffer)
            {
            m_CommentPositions.push_back(pair);
            }
          }
        if ( *inch == '/' )
          {
          if ( cc != std::string::npos && *(inch-1) == '*' )
            {
            endOfComment = cc+1;
            inComment = false;
            copyChar = false;
            PairType pair(beginOfComment, endOfComment);
            if(!buffer)
              {
              m_CommentPositions.push_back(pair);
              }
            }
          }
        }
      }
    if ( inComment )
      {
      copyChar = false;
      }
    if ( copyChar )
      {
      outBuffer.push_back(*inch);
      }
    inch++;
    }
  outBuffer.push_back(0);
  if(buffer)
    {
    return &*outBuffer.begin();
    }
  else
    {
    m_BufferNoComment = &*outBuffer.begin();
    }

  return "";
}

/** Find the constructor in the file */
size_t Parser::FindConstructor(const std::string & buffer, const std::string & className, 
                               bool headerfile, size_t startPos) const
{
  std::string constructor = "";
  
  if(headerfile)
    {
    constructor = " ";
    }
  else
    {
    constructor = ":";
    }
  constructor += className;
  size_t pos = buffer.find(constructor.c_str(),startPos);
  size_t pos2 = buffer.find("(",pos);
  size_t pos3 = buffer.find(")",pos2);
  
  // if it's not the destructor ...
  /*std::string dest = "~";
  dest += className;
  long destructor = buffer.find(dest.c_str(),0);*/
  while(pos != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos)
    {
    //if(pos != destructor+1) // we have the constructor
      {
      // we look if we have only spaces or \n between pos pos2 and pos3
      std::string val = buffer.substr(pos+className.size()+1,pos2-pos-className.size()-1);
      bool ok = true;
      for(size_t i=0;i<val.size();i++)
        {
        if(val[i] != ' ' || val[i] != '\n')
          {
          ok =false;
          }
        }

      bool ok2 = false;
      if(ok)
        {
        ok2 = true;
        val = buffer.substr(pos2+1,pos3-pos2-1);       
        for(size_t i=0;i<val.size();i++)
          {      
          if(val[i] != ' ' || val[i] != '\n')
            {
            ok2 =false;
            }
          }
        }

      // if ok2 then we check if the constructor is implemented
      if(ok2)
        {
        size_t pos4 = buffer.find(";",pos3);
        size_t pos5 = buffer.find("{",pos3);

        if(pos5<pos4)
          {
          return pos5;
          }
        else
          {
          return std::string::npos;
          }
        }
      }
    std::string constructor = "";
    if(headerfile)
      {
      constructor = " ";
      }
    else
      {
      constructor = ":";
      }
    constructor += className;
    pos = buffer.find(constructor.c_str(),pos+1);
    pos2 = buffer.find("(",pos);
    pos3 = buffer.find(")",pos2);
    }

  return std::string::npos;
}

/** Find the closing char given the position of the opening char */
size_t Parser::FindClosingChar(char openChar, char closeChar, 
                               size_t pos,bool noComment,std::string buffer) const
{
  std::string stream = buffer;
  if(buffer.size() == 0)
    {
    stream = m_Buffer.c_str();
    if(noComment)
      {
      stream = m_BufferNoComment.c_str();
      }
    }

  size_t open = 1;
  for(size_t i=pos+1;i<stream.length();i++)
    {
    if(stream[i] == openChar || stream[i] == closeChar)
      {
      bool skip = false;
      // We want to check that we are not in the #if/#else/#endif thing
      IfElseEndifListType::const_iterator itLS = m_IfElseEndifList.begin();
      while(itLS != m_IfElseEndifList.end())
        {
        size_t j = i;
        if(noComment)
          {
          j = this->GetPositionWithoutComments(i);
          }
        if(j>(*itLS).first && j<(*itLS).second)
          {
          skip = true;
          break;
          }
        itLS++;
        }
    
      if(!skip)
        {
        if(stream[i] == openChar)
          {
          open++;
          }
        else if(stream[i] == closeChar)
          {
          open--;
          }
        }
      }
    if(open == 0)
      {
      return i;
      }
    }
  return std::string::npos; // closing char not found
}


/** Compute the list of #if/#else/#endif 
 *  In the case we have #if/#else/#endif we want to ignore the #else section
 *  if we have some '{' not closed
 *  #if 
 *   {
 *  #else
 *   {
 * #endif */
void Parser::ComputeIfElseEndifList()
{
  m_IfElseEndifList.clear();
  size_t posSharpElse = m_BufferNoComment.find("#else",0);
  while(posSharpElse != std::string::npos)
    {
    // Find the corresponding #endif
    size_t posSharpEndif = m_BufferNoComment.find("#endif",posSharpElse);
    while(posSharpEndif != std::string::npos)
      {
      // Look if we have any #if between the #else and #endif
      // Count the number of #if and #endif
      int nIf = 0;
      size_t posIf = m_BufferNoComment.find("#if",posSharpElse);
      while(posIf<posSharpEndif && posIf!=std::string::npos)
        {
        posIf = m_BufferNoComment.find("#if",posIf+1);
        nIf++;
        }
        
      int nEndIf = 0;
      size_t posEndif = m_BufferNoComment.find("#endif",posSharpElse);
      while(posEndif<posSharpEndif && posEndif!=std::string::npos)
        {
        posEndif = m_BufferNoComment.find("#endif",posEndif+1);
        nEndIf++;
        }
       
      if(nEndIf == nIf)
        {
        break;
        }
        
      // Then continue to search*/
      posSharpEndif = m_BufferNoComment.find("#endif",posSharpEndif+1); 
      }
    
    if(posSharpEndif != std::string::npos)
      {
      // Search for the corresponding if
      size_t posSharpIf = m_BufferNoComment.find("#if",0);
      while(posSharpIf != std::string::npos)
        {
        size_t posSharpIf2 = m_BufferNoComment.find("#if",posSharpIf+1);
        if(posSharpIf2>posSharpElse || posSharpIf2==std::string::npos)
          {
          break;
          }
        posSharpIf = posSharpIf2;
        }

      if(posSharpIf != std::string::npos)
        {
        // We check if the total number of '{' is equal to the total number of '}'
        // in the #if/#else/#endif section
        int nOpen = 0;
        int nClose = 0;

        size_t posOpen = m_BufferNoComment.find("{",posSharpIf);
        while(posOpen != std::string::npos && posOpen<posSharpEndif)  
          {
          nOpen++;
          posOpen = m_BufferNoComment.find("{",posOpen+1);
          }

        size_t posClose = m_BufferNoComment.find("}",posSharpIf);
        while(posClose != std::string::npos && posClose<posSharpEndif)  
          {
          nClose++;
          posClose = m_BufferNoComment.find("}",posClose+1);
          }
        
        if(nOpen != nClose)
          {
          IfElseEndifPairType p;
          p.first = this->GetPositionWithComments(posSharpElse-1);
          p.second = this->GetPositionWithComments(posSharpEndif);
          m_IfElseEndifList.push_back(p);
          }
        }     
      }
    posSharpElse = m_BufferNoComment.find("#else",posSharpElse+1);
    }
}

/** Find the opening char given the position of the closing char */
size_t Parser::FindOpeningChar(char closeChar, char openChar,size_t pos,bool noComment) const
{  
  if(pos == std::string::npos)
    {
    return std::string::npos;
    }

  std::string stream = m_Buffer.c_str();
  if(noComment)
    {
    stream = m_BufferNoComment.c_str();
    }

  long int close = 1;
  for(size_t i=pos-1;i!=std::string::npos;i--)
    { 
    if(stream[i] == closeChar)
      {
      close++;
      }
    else if(stream[i] == openChar)
      {
      close--;
      }
    if(close == 0)
      {
      return i;
      }
    }
  return std::string::npos; // opening char not found
}

/** Generate the fixed file */
void Parser::GenerateFixedFile()
{
  if(!this->m_FixFile)
    {
    return;
    }

  if(m_ErrorList.size() == 0)
    {
    std::cout << "No error. Not generating corrected file." << std::endl;
    return;
    }

  std::string filename = kwssys::SystemTools::GetFilenameWithoutExtension(m_Filename.c_str());
  filename += ".fixed";
  filename += kwssys::SystemTools::GetFilenameExtension(m_Filename.c_str());

  std::cout << "Generating corrected file: " << filename.c_str() << std::endl;

  std::ofstream file;
  file.open(filename.c_str(), std::ios::binary | std::ios::out);
  if(!file.is_open())
    {
    std::cout << "Cannot open file for writing: " <<  std::endl;
    return;
    }

  file << this->m_FixedBuffer;
  
  file.close();
}

/** Functions to deal with the fixed buffer */
void Parser::ReplaceCharInFixedBuffer(size_t pos,size_t size,const char* replacingString)
{
  std::vector<PairType>::const_iterator it = m_FixedPositions.begin();
  while(it != m_FixedPositions.end())
    {
    if((*it).first<pos)
      {
      pos += (*it).second;
      }
    it++;
    }
  m_FixedBuffer.replace(pos,size,replacingString);

  // Keep track of the current position history
  size_t sizeNewChar = strlen(replacingString);
  if(sizeNewChar != size)
    {
    PairType p;
    p.first = pos;
    p.second = sizeNewChar-size;
    m_FixedPositions.push_back(p);
    }
}

} // end namespace kws
