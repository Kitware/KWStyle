/*=========================================================================

  Program:   ITKXML
  Module:    kwsParser.cxx
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
#include <itksys/SystemTools.hxx>
#include <itksys/Directory.hxx>
#include <itksys/RegularExpression.hxx>
#include <algorithm>

namespace kws {

/** Constructor */
Parser::Parser()
{
  m_HeaderFilename = "";

  for(unsigned int i=0;i<NUMBER_ERRORS+1;i++)
    {
    m_TestsDone[i] = false;
    }
}

/** Destructor */
Parser::~Parser()
{
}

/** Return if a test has been performed */
bool Parser::HasBeenPerformed(unsigned int test) const
{
  return m_TestsDone[test];
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
    sprintf(val,"%d",(*it).number);
    output += val;
    delete [] val;
    output += " (";
    val = new char[10];
    sprintf(val,"%d",(*it).line);
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
  return ErrorTag[number-1];
}

/** Return the information vector as a string */
std::string Parser::GetInfo()
{
  std::string output = "";
  std::vector<Info>::const_iterator it = m_InfoList.begin();
  while(it != m_InfoList.end())
    {
    output += "Info #";
    char* val = new char[10];
    sprintf(val,"%d",(*it).number);
    output += val;
    delete [] val;
    output += " (";
    val = new char[10];
    sprintf(val,"%d",(*it).line);
    output += val;
    delete [] val;
    output += ") ";
    output += (*it).description;
    output += "\n";
    it++;
    }
  return output;
}


/** Check if the #ifndef/#define is defined correctly for the class 
 *  match can contain <NameOfClass> and <Extension> */
bool Parser::CheckIfNDefDefine(const char* match)
{
  m_TestsDone[NDEFINE] = true;

  bool hasError = false;
  bool notDefined = false;

  // Find the first word in the file
  long int pos = m_BufferNoComment.find("#ifndef",0);
  if(pos == -1)
    {
    notDefined = true;
    }
  else
    {
    for(int i=0;i<pos;i++)
      {
      if((m_BufferNoComment[i] != ' ')
        && (m_BufferNoComment[i] != '\r')
        && (m_BufferNoComment[i] != '\n')
        )
        {
        notDefined = true;
        }
      }
    }

  long int definepos = pos;

  if(notDefined)
    {
    Error error;
    error.line = this->GetLineNumber(0,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#ifndef not defined";
    m_ErrorList.push_back(error);
    return false;
    }

  // Find the word after #ifndef
  pos += 8;
 
  while(m_BufferNoComment[pos] == ' ')
    {
    pos++;
    }
  long int begin = pos;
  while((m_BufferNoComment[pos] != ' ') 
    &&(m_BufferNoComment[pos] != '\r')
    &&(m_BufferNoComment[pos] != '\n')
    )
    {
    pos++;
    }
  long int end = pos;
  std::string ifndef = m_BufferNoComment.substr(begin,end-begin);

  // Find the word after #define
  pos = m_BufferNoComment.find("#define",end);
  
  if(pos == -1)
    {
    Error error;
    error.line = this->GetLineNumber(end,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#define not defined";
    m_ErrorList.push_back(error);
    return false;
    }
  
  pos += 7; 
  while(m_BufferNoComment[pos] == ' ')
    {
    pos++;
    }
  begin = pos;
  while((m_BufferNoComment[pos] != ' ') 
    &&(m_BufferNoComment[pos] != '\r')
    &&(m_BufferNoComment[pos] != '\n')
    )
    {
    pos++;
    }
  end = pos;
  std::string define = m_BufferNoComment.substr(begin,end-begin);

  if(ifndef != define)
    {
    Error error;
    error.line = this->GetLineNumber(end,true);
    error.line2 = error.line;
    error.number = NDEFINE;
    error.description = "#define does not match #ifndef";
    m_ErrorList.push_back(error);
    return false;
    }

  if(m_Filename == "")
    {
    std::cout << "CheckIfNDefDefine() : m_Filename shoud be set" << std::endl;
    return false;
    }

  long int point = m_Filename.find_last_of(".");
  long int slash = m_Filename.find_last_of("/");

  if(slash == -1)
    {
    slash = 0;
    }

  std::string nameofclass = m_Filename.substr(slash+1,point-slash-1);
  std::string extension = m_Filename.substr(point+1,m_Filename.size()-point-1);

  // construct the string
  std::string toMatch = match;
  pos = toMatch.find("<NameOfClass>");
  if(pos != -1)
    {
    toMatch.replace(pos,13,nameofclass);
    }
  pos = toMatch.find("<Extension>");
  if(pos != -1)
    {
    toMatch.replace(pos,11,extension);
    }


  if(ifndef != toMatch)
    {   
    Error error;
    error.line = this->GetLineNumber(definepos,true);
    error.line2 = this->GetLineNumber(end,true);
    error.number = NDEFINE;
    error.description = "#ifndef/#define does not match expression";
    m_ErrorList.push_back(error);
    return false;
    }

  return !hasError;
}

/** Check header given a template filename 
 *  The template should have '<NA>\n' tag to avoid checking the rest of the line
 *  or <NA> to skip a word.
 *  The header should also be at the beginning of the file */
bool Parser::CheckHeader(const char* filename, bool considerSpaceEOL,bool useCVS)
{
  m_TestsDone[HEADER] = true;

  bool hasError = false;
  if(!filename)
    {
    std::cout << "CheckHeader(): Please specify an header file" << std::endl;
    return false;
    }
  
  // Read the file
  std::ifstream file;
  file.open(filename, std::ios::binary | std::ios::in);
  if(!file.is_open())
    {
    std::cout << "Cannot open file: " << filename << std::endl;
    return false;
    }

  file.seekg(0,std::ios::end);
  unsigned long fileSize = file.tellg();
  file.seekg(0,std::ios::beg);

  char* buf = new char[fileSize+1];
  file.read(buf,fileSize);
  buf[fileSize] = 0;
  std::string buffer(buf);
  buffer.resize(fileSize);
  file.close();
  delete [] buf;

  m_HeaderFilename = filename;
  
  // Check the file char by char
  std::string::const_iterator ith = buffer.begin();
  std::string::const_iterator it = m_Buffer.begin();

  unsigned int pos = 0;
  unsigned int posh = 0;
  int line = -1;

  while((ith != buffer.end()) && (it != m_Buffer.end()))
    {
    // if we have cvs
    if((*ith == '$') && useCVS)
      {
      ith++;
      posh++;
      it++;
      pos++;
      while(((*ith) != '$') && (ith != buffer.end()))
        {
        ith++;
        posh++;
        }
      while(((*it) != '$') && (it != m_Buffer.end()))
        {
        pos++;
        it++;
        }
      //continue;
      }
     
    if((*it) != (*ith))
      {
      // Check if we have a <NA> tag
      if((*ith) == '<')
        {
        long int pos2 = buffer.find("<NA>",posh);
        long int pos3 = buffer.find("<NA>\n",posh);

        // We skip the line
        if(pos3 == posh)
          {
          while(((*ith) != '\n') && (ith != buffer.end()))
            {
            ith++;
            posh++;
            }
          while(((*it) != '\n') && (it != m_Buffer.end()))
            {
            pos++;
            it++;
            }
          continue;
          }
        // if we have the tag we skip the word
        else if(pos2 == posh)
          {
          while(((*ith) != ' ') && (ith != buffer.end()))
            {
            ith++;
            posh++;
            }
          while(((*it) != ' ') && (it != m_Buffer.end()))
            {
            pos++;
            it++;
            }
          continue;
          }
        }
      // if we should not check the spaces at the end of line
      else if( (!considerSpaceEOL)
        && ((*ith == ' ') || (*it == ' '))
        )
        {
        // search if we are effectively at the end of the line
        bool isAtEnd = true;
        if(*it == ' ')
          {
          std::string::const_iterator ittemp = it;
          while((ittemp != m_Buffer.end()) && ((*ittemp) != '\n'))
            {
            if(*ittemp != ' ')
              {
              isAtEnd = false;
              break;
              }
            ittemp++;
            }
          }
        else if(*ith == ' ')
          {
          std::string::const_iterator ittemp = ith;
          while((ittemp != buffer.end()) && ((*ittemp) != '\n'))
            {
            if(*ittemp != ' ')
              {
              isAtEnd = false;
              break;
              }
            ittemp++;
            }
          }

        // If we are at the end we skip the line
        if(!isAtEnd)
          {
          while((ith != buffer.end()) && ((*ith) != '\n'))
            {
            ith++;
            posh++;
            }
          while((it != m_Buffer.end()) && ((*it) != '\n'))
            {
            pos++;
            it++;
            }
          continue;
          }
        }

      // Report the error
      hasError = true;

      // We report the wrong word and the line
      int l = this->GetLineNumber(pos);
      if(l != line)
        {
        line = l;
        // Find the word
        long int poshw = buffer.find(' ',posh);
        long int poshw2 = buffer.find('\n',posh);
        std::string wordh = "";
        if(poshw < poshw2)
          {
          wordh = buffer.substr(posh,poshw-posh);
          }
        else if (poshw2 != -1)
          {
          wordh = buffer.substr(posh,poshw2-posh);
          }

        // Find the word
        long int posw = m_Buffer.find(' ',pos);
        long int posw2 = m_Buffer.find('\n',pos);
        std::string word = "";
        if(posw < posw2)
          {
          word = m_Buffer.substr(pos,posw-pos);
          }
        else if (poshw2 != -1)
          {
          word = m_Buffer.substr(pos,posw2-pos);
          }

        if(word == wordh)
          {
          wordh = "wrond ident";
          }
       
        if(word == " ")
          {
          word = "[space]";
          }
        if(wordh == " ")
          {
          wordh = "[space]";
          }
       if(word == "\r")
          {
          word = "[end of line]";
          }
        if(wordh == "\r")
          {
          wordh = "[end of line]";
          }
        
        if(word[0] == 0)
          {
          word = "[no char]";
          }
        if(wordh[0] == 0)
          {
          wordh = "[no char]";
          }

        Error error;
        error.line = line;
        error.line2 = error.line;
        error.number = HEADER;
        error.description = "Header mismatch: ";
        error.description += word;
        error.description += " (";
        error.description += wordh;
        error.description += ")";
        m_ErrorList.push_back(error);
        hasError = true;

        // We skip that line   
        while((ith != buffer.end()) && ((*ith) != '\n'))
          {
          ith++;
          posh++;
          }
        while((it != m_Buffer.end()) && ((*it) != '\n'))
          {
          pos++;
          it++;
          }
        }
      }

    if(ith != buffer.end())
      {
      posh++;
      ith++;
      }

    if(it != m_Buffer.end())
      {
      pos++;
      it++;
      }
    }

  if(it == m_Buffer.end())
    {
    Error error;
    error.line = 1;
    error.line2 = error.line;
    error.number = HEADER;
    error.description = "The header is incomplete";
    m_ErrorList.push_back(error);
    hasError = true;
    }

  return !hasError;
}


/** Check the number of space between the end of the declaration
 *  and the semicolon */
bool Parser::CheckSemicolonSpace(unsigned long max)
{
  m_TestsDone[SEMICOLON_SPACE] = true;

  bool hasError = false;
  long int posSemicolon = m_BufferNoComment.find(";",0);
  while(posSemicolon != -1)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    unsigned long space = 0;
    while(i>=0)
      {
      if(m_BufferNoComment[i] == ' ')
        {
        space++;
        if(space > max)
          {
          Error error;
          error.line = this->GetLineNumber(posSemicolon,true);
          error.line2 = error.line;
          error.number = SEMICOLON_SPACE;
          error.description = "Number of spaces before semicolon exceed: ";
          char* val = new char[10];
          sprintf(val,"%d",space);
          error.description += val;
          error.description += " (max=";
          delete [] val;
          val = new char[10];
          sprintf(val,"%d",max);
          error.description += val;
          error.description += ")";
          delete [] val;
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      else
        {
        break;
        }
      i--;
      }
    
    posSemicolon = m_BufferNoComment.find(";",posSemicolon+1);
    }
  return !hasError;
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
     return m_Buffer.substr(prec,pos-prec-1);
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


/** Check the number of character per line */
bool Parser::CheckLineLength(unsigned long max)
{
  m_TestsDone[LINE_LENGTH] = true;
  m_Positions.clear();
  unsigned long total = m_Buffer.size();
  unsigned long i = 0;
  unsigned long j = 1;
  bool hasError = false;
  while(i<total)
    {
    // extract the line
    std::string line = m_Buffer.substr(i+1,m_Buffer.find("\n",i+1)-i-1);
    m_Positions.push_back(i);
    if(line.length() > max)
      {
      Error error;
      error.line = j;
      error.line2 = error.line;
      error.number = LINE_LENGTH;
      error.description = "Line length exceed ";
      char* val = new char[10];
      sprintf(val,"%d",line.length());
      error.description += val;
      error.description += " (max=";
      delete [] val;
      val = new char[10];
      sprintf(val,"%d",max);
      error.description += val;
      error.description += ")";
      delete [] val;
      m_ErrorList.push_back(error);
      hasError = true;
      }
    j++;
    i += line.length()+1;
    }

  m_Positions.push_back(total-1);
    
  return !hasError;
}

/** Find the previous word given a position */
std::string Parser::FindPreviousWord(long int pos) const
{
  long i=pos;

  while(m_BufferNoComment[i] != ' ' && i>0)
    {
    i--;
    }

  bool inWord = true;
  bool first = false;
  std::string ivar = "";
  while(i>=0 && inWord)
    {  
    if(m_BufferNoComment[i] != ' ' && m_BufferNoComment[i] != '\n')
      {
      std::string store = ivar;
      ivar = m_BufferNoComment[i];
      ivar += store;
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
    i--;
    }
  return ivar;
}

/** Find the next word given a position */
std::string Parser::FindNextWord(long int pos) const
{
  long i=pos;

  // we go to the next space
  while(m_BufferNoComment[i] != ' ' && i<(long)m_BufferNoComment.size())
    {
    i++;
    }

  bool inWord = true;
  bool first = false;
  std::string ivar = "";
  while(i<(long)m_BufferNoComment.size() && inWord)
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
unsigned long Parser::GetPositionInLine(unsigned long pos)
{
  long begin = pos;
  while(m_BufferNoComment[begin]!='\n')
    {
    begin--;
    }
  return pos-begin;
}


/** Check if the typedefs of the class are correct */
bool Parser::CheckTypedefs(const char* regEx, bool alignment)
{
  // First we need to find the typedefs
  // typedef type MyTypeDef;
  bool hasError = false;

  itksys::RegularExpression regex(regEx);

  long int previousline = 0;
  long int previouspos = 0;
  long int pos = 0;
  while(pos!= -1)
    {
    long int beg = 0;
    std::string var = this->FindTypedef(pos+1,m_BufferNoComment.size(),pos,beg);
    
    if(var == "")
      {
      continue;
      }
    // Check the alignment if specified
    if(alignment)
      {
      // Find the position in the line
      unsigned long l = this->GetPositionInLine(beg);
      unsigned long line = this->GetLineNumber(beg,false);
      
      // if the typedef is on a line close to the previous one we check
      if(line-previousline<=2)
        {
         if(l!=previouspos)
           {
           Error error;
           error.line = this->GetLineNumber(beg,true);
           error.line2 = error.line;
           error.number = TYPEDEF_ALIGN;
           error.description = "Type definition (" + var + ") is not aligned with the previous one";
           m_ErrorList.push_back(error);
           hasError = true;
           }
        }
      else
        {
        previouspos = l;
        }
      previousline = line;
      }

    if(!regex.find(var))
      {
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = TYPEDEF_REGEX;
      error.description = "Type definition (" + var + ") doesn't match regular expression";
      m_ErrorList.push_back(error);
      hasError = true;
      }
    }
  return !hasError;
}

/** Find a typedef  in the source code */
std::string Parser::FindTypedef(long int start, long int end,long int & pos,long int & beg)
{
  long int posSemicolon = m_BufferNoComment.find(";",start);
  if(posSemicolon != -1 && posSemicolon<end)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    bool inWord = true;
    bool first = false;
    std::string ivar = "";
    while(i>=0 && inWord)
      {
      if(m_BufferNoComment[i] != ' ')
        {
        if((m_BufferNoComment[i] == '}')
          || (m_BufferNoComment[i] == ')')
          || (m_BufferNoComment[i] == ']')
          || (m_BufferNoComment[i] == '\n')
          )
          {
          inWord = false;
          }
        else
          {
          std::string store = ivar;
          ivar = m_BufferNoComment[i];
          ivar += store;
          beg = i;
          inWord = true;
          first = true;
          }
        }
      else // we have a space
        {
        if(first)
          {
          inWord = false;
          }
        }
      i--;
      }
    pos = posSemicolon;

    // We find the words until we find a semicolon
    long int p = pos;

    std::string pword = this->FindPreviousWord(p);
    bool isTypedef = false;
    while((pword.find(";") == -1) && (p>0))
      {
      if(pword.find("typedef") != -1)
        {
        isTypedef = true;
        break;
        }
      p -= pword.size();
      pword = this->FindPreviousWord(p);
      }
    if(isTypedef)
      {
      return ivar;
      }
    }

  pos = -1;
  return "";
}

/** Find an ivar in the source code */
std::string Parser::FindInternalVariable(long int start, long int end,long int & pos)
{
  long int posSemicolon = m_BufferNoComment.find(";",start);
  if(posSemicolon != -1 && posSemicolon<end)
    {
    // We try to find the word before that
    unsigned long i=posSemicolon-1;
    bool inWord = true;
    bool first = false;
    std::string ivar = "";
    while(i>=0 && inWord)
      {
      if(m_BufferNoComment[i] != ' ')
        {
        if((m_BufferNoComment[i] == '}')
          || (m_BufferNoComment[i] == ')')
          || (m_BufferNoComment[i] == ']')
          || (m_BufferNoComment[i] == '\n')
          )
          {
          inWord = false;
          }
        else
          {
          std::string store = ivar;
          ivar = m_BufferNoComment[i];
          ivar += store;
          inWord = true;
          first = true;
          }
        }
      else // we have a space
        {
        if(first)
          {
          inWord = false;
          }
        }
      i--;
      }
    pos = posSemicolon;

    // We find the words until we find a semicolon
    long int p = pos;

    std::string pword = this->FindPreviousWord(p);
    bool isTypedef = false;
    while((pword.find(";") == -1) && (p>0))
      {
      if(pword.find("typedef") != -1)
        {
        isTypedef = true;
        break;
        }
      p -= pword.size();
      pword = this->FindPreviousWord(p);
      }
    if(!isTypedef)
      {
      return ivar;
      }
    }

  pos = -1;
  return "";
}

/** Given the position without comments return the position with the comments */
long int Parser::GetPositionWithComments(long int pos)
{
  std::vector<PairType>::const_iterator it = m_CommentPositions.begin();
  while(it != m_CommentPositions.end())
    {
    if((pos>=(*it).first))
      {
      pos += ((*it).second-(*it).first)+2;
      }
    else
      {
      break;
      }
    it++;
    }

  return pos;
}

/** Return the line number in the source code given the character position */
long int Parser::GetLineNumber(long int pos,bool withoutComments)
{
  
  // if we have comments we add them to the list
  if(withoutComments)
    {
    pos = this->GetPositionWithComments(pos);
    }

  unsigned int i=0;
  std::vector<long int>::const_iterator it = m_Positions.begin();
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

/** Check if the file contains tabs */
bool Parser::CheckTabs()
{
  bool hasError = false;
  long int pos = m_Buffer.find('\t',0);    
  long int line = 0;

  // Show only one tab per line
  while(pos != -1)
    {
    if(this->GetLineNumber(pos,false) != line)
      {
      line = this->GetLineNumber(pos,false);
      Error error;
      error.line = line; 
      error.line2 = error.line;
      error.number = TABS;
      error.description = "Tabs identified";
      m_ErrorList.push_back(error);
      }
    pos = m_Buffer.find('\t',pos+1);
    hasError = true;  
    }

  return !hasError;
}

/** Check the indent size */
bool Parser::CheckIndent(IndentType itype,unsigned long size,bool doNotCheckHeader)
{
  bool hasError = false;
  unsigned long pos = 0;
  unsigned int currentPosition = 0;
  std::string::const_iterator it = m_Buffer.begin();

  bool checkSpaces = false;
  int count = 0;
  unsigned long line = 0;

  // We construct a vector of position pair brackets
  // The first one is the position of the char to avoid
  // The second one can take different values
  // 0: means we totally ignore the bracket
  // 1: means that the bracket is respecting the previous ident but the rest is respecting the correct ones
  typedef std::pair<long int,int> PairType;
  std::vector<PairType> ignoreBrackets;

  // namespace
  long int posNamespace = m_Buffer.find("namespace",0);
  while(posNamespace!=-1)
    {
    long int posNamespace1 = m_Buffer.find("{",posNamespace);
    if(posNamespace1 != -1)
      {
      long int posNamespace2 = m_Buffer.find(";",posNamespace);
      if(posNamespace2 > posNamespace1)
        {
        PairType pair(posNamespace1,0); 
        ignoreBrackets.push_back(pair);
        PairType pair2(this->FindClosingChar('{','}',posNamespace1),0);
        ignoreBrackets.push_back(pair2);
        }
      }
    posNamespace = m_Buffer.find("namespace",posNamespace+1);
    }

  // class
  long int posClass = m_BufferNoComment.find("class",0);
  while(posClass!=-1)
    {
    if(!IsBetweenChars('<','>',posClass))
      {
      bool valid = true;
      // We should get a { before a ;
      long int i = posClass+4;
      while((m_BufferNoComment[i] != '{')
         && (i<(long)m_BufferNoComment.size())
         )
        {
        if(m_BufferNoComment[i] == ';')
          {
          valid = false;
          break;
          }
        i++;
        }

      if(valid && m_BufferNoComment[i] == '{')
        {
        // translate the position in the buffer position;
        long int posClassComments = this->GetPositionWithComments(i);
        PairType pair(posClassComments,1); 
        ignoreBrackets.push_back(pair);
        PairType pair2(this->FindClosingChar('{','}',posClassComments),1); 
        ignoreBrackets.push_back(pair2);
        }
      }
    posClass = m_BufferNoComment.find("class",posClass+1);
    }
  
  // Some words should be indented  as the previous indent
  std::vector<unsigned long> previousIndentVector;  
  long int posPrev = m_Buffer.find("public:",0);
  while(posPrev!=-1)
    {
    previousIndentVector.push_back(posPrev);
    posPrev = m_Buffer.find("public:",posPrev+1);
    }
  posPrev = m_Buffer.find("private:",0);
  while(posPrev!=-1)
    {
    previousIndentVector.push_back(posPrev);
    posPrev = m_Buffer.find("private:",posPrev+1);
    }
  posPrev = m_Buffer.find("protected:",0);
  while(posPrev!=-1)
    {
    previousIndentVector.push_back(posPrev);
    posPrev = m_Buffer.find("protected:",posPrev+1);
    }

  char type = ' ';
  if(itype == TABS) {type = '\t';}

  // If we do not want to check the header
  if((m_HeaderFilename.size() > 0) && doNotCheckHeader)
    {
    std::ifstream file;
    file.open(m_HeaderFilename.c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open file: " << m_HeaderFilename << std::endl;
      return false;
      }

    file.seekg(0,std::ios::end);
    unsigned long fileSize = file.tellg();
    file.close();
    for(unsigned int i=0;i<fileSize;i++)
      {
      if(it != m_Buffer.end())
        {
        pos++;
        it++;
        }
      }
    }

  while(it != m_Buffer.end())
    {
    int ignoreBracket = 0;
    std::vector<PairType>::const_iterator it1 = ignoreBrackets.begin();

    while(it1 != ignoreBrackets.end())
      {
      if((*it1).first == pos)
        {
        ignoreBracket = 1+(*it1).second;
        break;
        }
      it1++;
      }

    if(checkSpaces && ((*it) != '\n') && ((*it) != '{') && ((*it) != '}'))
      {
      if((*it) == type)
        {
        count++;
        }
      else
        {
        // We check if the current word should be at the previous indent
        bool previousIndent = false;
        std::vector<unsigned long>::const_iterator itPrev = previousIndentVector.begin();
        while(itPrev != previousIndentVector.end())
          {
          if(*itPrev == pos)
            {
            previousIndent = true;
            break;
            }
          itPrev++;
          }

        if(previousIndent)
          {
          if(count != currentPosition-size)
            {
            unsigned long l =  this->GetLineNumber(pos,false);
            if(l != line)
              {
              Error error;
              error.line = l;
              error.line2 = error.line;
              error.number = INDENT;
              error.description = "Indent is wrong ";
              char* val = new char[10];
              sprintf(val,"%d",count); 
              error.description += val;
              error.description += " (should be ";
              delete [] val;
              val = new char[10];
              sprintf(val,"%d",currentPosition);
              error.description += val;
              error.description += ")";
              delete [] val;
              m_ErrorList.push_back(error);      
              line = l;
              hasError = true;
              }
            count = 0;
            checkSpaces = false;
            }
          }
        // if the line is empty we do not care
        else if(((count != currentPosition) && (*it!=13)))
          {
          bool commentError = false;
          unsigned long l =  this->GetLineNumber(pos,false);
          
          if(this->IsInComments(pos))
            {
            commentError = true;

            // We check how much space we have in the middle section
            unsigned int nSpaceMiddle = 0;
            while(m_CommentMiddle[nSpaceMiddle] == type)
              {
              nSpaceMiddle++;
              }

             if(((*it) == m_CommentMiddle[nSpaceMiddle])
              && (count != currentPosition+nSpaceMiddle)
              )
              {
              commentError = false;
              }

            // We check how much space we have in the end section
            unsigned int nSpaceEnd = 0;
            while(m_CommentEnd[nSpaceEnd] == type)
              {
              nSpaceEnd++;
              }

             if(((*it) == m_CommentEnd[nSpaceEnd])
              && (count != currentPosition+nSpaceEnd)
              )
              {
              commentError = false;
              }
            }

          if(l != line && !commentError)
            {
            Error error;
            error.line = l;
            error.line2 = error.line;
            error.number = INDENT;
            error.description = "Indent is wrong ";
            char* val = new char[10];
            sprintf(val,"%d",count); 
            error.description += val;
            error.description += " (should be ";
            delete [] val;
            val = new char[10];
            sprintf(val,"%d",currentPosition);
            error.description += val;
            error.description += ")";
            delete [] val;
            m_ErrorList.push_back(error);      
            line = l;
            hasError = true;
            }
          }
        count = 0;
        checkSpaces = false;
        }
      }
    if(((*it) == '{') && (ignoreBracket != 1))
      {
      currentPosition += size;
      // Check if the current bracket is at the right position
      int j=1;
      bool missingNewLine = false;
      while(m_Buffer[pos-j] != '\n')
        { 
        if((m_Buffer[pos-j] != type))
          {
          missingNewLine = true;
          }
        j++;
        }

      unsigned long l =  this->GetLineNumber(pos,false);
      register
      long int position = currentPosition;
      if(ignoreBracket == 2){position -= size;}

      if((j-1)!=position)
        {
        Error error;
        error.line = l;
        error.line2 = error.line;
        error.number = INDENT;
        error.description = "Indent is wrong ";
        char* val = new char[10];
        sprintf(val,"%d",j-1);
        error.description += val;
        error.description += " (should be ";
        delete [] val;
        val = new char[10];
        sprintf(val,"%d",currentPosition);
        error.description += val;
        if(missingNewLine)
          {
          error.description += ", missing new line";
          }
        error.description += ")";
        delete [] val;
        m_ErrorList.push_back(error);
        }
      }
    else if ((*it) == '}')
      {

      if(ignoreBracket != 1)
        {
        currentPosition -= size;
        }
      
      // We check if the bracket is ok
      if(ignoreBracket == 2)
        {
        if(count != currentPosition)
          {
          unsigned long l =  this->GetLineNumber(pos,false);
          if(l != line)
            {
            Error error;
            error.line = l;
            error.line2 = error.line;
            error.number = INDENT;
            error.description = "Indent2 is wrong ";
            char* val = new char[10];
            sprintf(val,"%d",count); 
            error.description += val;
            error.description += " (should be ";
            delete [] val;
            val = new char[10];
            sprintf(val,"%d",currentPosition);
            error.description += val;
            error.description += ")";
            delete [] val;
            m_ErrorList.push_back(error);      
            line = l;
            hasError = true;
            }
          count = 0;
          checkSpaces = false;
          }
        }
     
      // if ignore bracket is 1 or 2 we ignore the rest of the line
      // we look at the next space or \n or eof
      if(ignoreBracket != 0)
        {
        do
          {
          it++;
          pos++;
          }
        while( ((*it) != ' ') && ((*it) != '}') && ((*it) != '\n'));
        checkSpaces = true;
        count = 0;
        }
      }
    else if ((*it) == '\n')
      {
      checkSpaces = true;
      count = 0;
      }

    it++;
    pos++;
    }
 
 return !hasError;
}

/** Check if the end of the file has a new line */
bool Parser::CheckEndOfFileNewLine()
{
  bool hasError = false;
  
  // Check if the last character is an end of line
  if(m_Buffer[m_Buffer.size()-1] != '\n')
    {
    Error error;
    error.line = this->GetLineNumber(m_Buffer.size()-1,false);
    error.line2 = error.line;
    error.number = EOF_NEW_LINE;
    error.description = "No new line at the end of file";
    m_ErrorList.push_back(error);
    hasError = true;
    }

  // Check the number empty lines at the end of the file
  if((m_Buffer[m_Buffer.size()-1] == ' ') || (m_Buffer[m_Buffer.size()-1] == '\n')) 
    {
    long i = m_Buffer.size()-1;
    unsigned long numberOfEmptyLines = 0;
    while( ((m_Buffer[i] == '\n') ||  (m_Buffer[i] == ' ') || (m_Buffer[i] == '\r')) && (i>0))
      {
      if(m_Buffer[i] == '\n')
        {
        numberOfEmptyLines++;
        }
      i-=1;
      }
    
    if(numberOfEmptyLines>1)
      {
      // Maybe should be info and not error
      Error info;
      info.line2 = this->GetLineNumber(m_Buffer.size()-1,false)+1;
      info.line = info.line2-numberOfEmptyLines+2;      
      info.number = EOF_NEW_LINE;
      info.description = "Number of empty lines at the end of files: ";
      char* val = new char[10];
      sprintf(val,"%d",numberOfEmptyLines);
      info.description += val;
      delete [] val;
      m_ErrorList.push_back(info);
      }
    }

  return !hasError;
}

/** Check the order of the declaration */
bool Parser::CheckDeclarationOrder(unsigned int posPublic, unsigned int posProtected, unsigned int posPrivate)
{
  long int publicFirst;
  long int publicLast;
  this->FindPublicArea(publicFirst,publicLast);
  
  long int protectedFirst;
  long int protectedLast;
  this->FindProtectedArea(protectedFirst,protectedLast);
  
  long int privateFirst;
  long int privateLast;
  this->FindPrivateArea(privateFirst,privateLast);
 
  bool hasError = false;

  // public v.s protected
  if( (posPublic > posProtected)
      && (posPublic != -1)
      && (posProtected != -1)
      && (protectedFirst > publicFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(protectedFirst,true);
    error.line2 = this->GetLineNumber(publicFirst,true);
    error.number = DECL_ORDER;
    error.description = "Public defined before Protected";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPublic > posProtected)
      && (posPublic != -1)
      && (posProtected != -1)
      && (protectedFirst > publicFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(protectedFirst,true);
     error.line2 = this->GetLineNumber(publicFirst,true);
     error.number = DECL_ORDER;
     error.description = "Protected defined before Public";
     m_ErrorList.push_back(error);
     hasError = true;
     }

  // protected v.s. private 
  if( (posPrivate > posProtected)
      && (posPrivate != -1)
      && (posProtected != -1)
      && (protectedFirst > privateFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(protectedFirst,true);
    error.line2 = this->GetLineNumber(privateFirst,true);
    error.number = DECL_ORDER;
    error.description = "Private defined before Protected";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPrivate > posProtected)
      && (posPrivate != -1)
      && (posProtected != -1)
      && (protectedFirst > privateFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(protectedFirst,true);
     error.line2 = this->GetLineNumber(privateFirst,true);
     error.number = DECL_ORDER;
     error.description = "Protected defined before Private";
     m_ErrorList.push_back(error);
     hasError = true;
     }

  // Public v.s. Private
  if( (posPublic > posPrivate)
      && (posPublic != -1)
      && (posPrivate != -1)
      && (privateFirst > publicFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(privateFirst,true);
    error.line2 = this->GetLineNumber(publicFirst,true);
    error.number = DECL_ORDER;
    error.description = "Public defined before Private";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPublic > posPrivate)
      && (posPublic != -1)
      && (posPrivate != -1)
      && (privateFirst > publicFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(privateFirst,true);
     error.line2 = this->GetLineNumber(publicFirst,true);
     error.number = DECL_ORDER;
     error.description = "Private defined before Public";
     m_ErrorList.push_back(error);
     hasError = true;
     }

   // Print the info
   Info info;
   info.line =  this->GetLineNumber(protectedFirst,true);
   info.line2 = this->GetLineNumber(publicFirst,true);
   info.number = DECL_ORDER;
   if(publicFirst == -1)
     {
     info.description = "This class doesn't have any public method";
     m_InfoList.push_back(info);
     }
   if(protectedFirst == -1)
     {
     info.description = "This class doesn't have any protected method";
     m_InfoList.push_back(info);
     }
   if(privateFirst == -1)
     {
     info.description = "This class doesn't have any private method";
     m_InfoList.push_back(info);
     }
    


  return !hasError;
}

/** Check if the internal variables of the class are correct */
bool Parser::CheckInternalVariables(const char* regEx)
{
  // First we need to find the parameters
  // float myParam;
  bool hasError = false;

  itksys::RegularExpression regex(regEx);

  // First we check in the public area
  long int publicFirst;
  long int publicLast;
  this->FindPublicArea(publicFirst,publicLast);
  long int pos = publicFirst;
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,publicLast,pos);

    if(var == "")
      {
      continue;
      }

    if(var.length() > 0)
      {
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = IVAR_PUBLIC;
      error.description = "Encapsulcation not preserved";
      m_ErrorList.push_back(error);
      hasError = true;
      
      if(!regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = IVAR_REGEX;
        error.description = "Internal variable (" + var + ") doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }

  // Second in the protected area
  long int protectedFirst;
  long int protectedLast;
  this->FindProtectedArea(protectedFirst,protectedLast);
  pos = protectedFirst;
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,protectedLast,pos);
    
    if(var == "")
      {
      continue;
      }

    if(var.length() > 0)
      {
      if(!regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = IVAR_REGEX;
        error.description = "Internal variable doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }


  // Third and last in the private area
  long int privateFirst;
  long int privateLast;
  this->FindPrivateArea(privateFirst,privateLast);
  pos = privateFirst;
  while(pos!= -1)
    {
    std::string var = this->FindInternalVariable(pos+1,privateLast,pos);

    if(var == "")
      {
      continue;
      }

    if(var.length() > 0)
      {
      if(!regex.find(var))
        {
        Error error;
        error.line = this->GetLineNumber(pos,true);
        error.line2 = error.line;
        error.number = IVAR_REGEX;
        error.description = "Internal variable doesn't match regular expression";
        m_ErrorList.push_back(error);
        hasError = true;
        }
      }
    }
  return !hasError;
}

void Parser::ReduceMultipleSpaces(std::string & buffer)
{
  // Reduce multiple spaces to one.
  size_t fPos=0;

  // Replace newlines by spaces.
  const size_t ns=buffer.size();
  for (fPos=0; fPos < ns; ++fPos){ if (buffer[ns]=='\n'){ buffer[ns]=' '; } }

  while ((fPos=buffer.find("       ")) != std::string::npos){ buffer.erase(fPos,6); if (fPos > 0){ fPos--; } }
  while ((fPos=buffer.find("     "))   != std::string::npos){ buffer.erase(fPos,4); if (fPos > 0){ fPos--; } }
  while ((fPos=buffer.find("  "))      != std::string::npos){ buffer.erase(fPos,1); if (fPos > 0){ fPos--; } }
}

//----------------------------------------------------------------------------------
/** Remove any unwanted char 
    Only at the end or the beginning of the word */
//----------------------------------------------------------------------------------
void Parser::RemoveChar(std::string & buffer, char val) const
{
  long pos = 0;

  //beginning
  while((buffer.find(val,0) == 0) && (pos != -1))
    {
    pos = buffer.find(val,0);
    if(pos != -1)
      {
      buffer = buffer.erase(pos,1);
      }
    }

  //end
   while((buffer.find(val,buffer.size()-1) == buffer.size()-1) && (pos != -1))
    {
    pos = buffer.find(val,buffer.size()-1);
    if(pos != -1)
      {
      buffer = buffer.erase(pos,1);
      }
    }


}

//----------------------------------------------------------------------------------
/** Remove any \n char in the buffer
 *  Only at the end or the beginning of the word */
//----------------------------------------------------------------------------------
void Parser::RemoveCtrlN(std::string & buffer) const
{
  long int ctrln = buffer.find("\n",0);

  while((ctrln != -1))
    {
    if(ctrln == 0)
      {
      buffer = buffer.erase(ctrln,1);
      }
    else
      {
      buffer = buffer.erase(ctrln-1,2);
      }
    ctrln = buffer.find("\n",ctrln+1);
  }
}


/** Find public area in source code. */
void Parser::FindPublicArea(long &before, long &after, size_t startPos) const
{
  before = 0;
  after = 0;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public", startPos);
  long priv = m_BufferNoComment.find("private", startPos);
  long protect = m_BufferNoComment.find("protected", startPos);

  if(priv == -1)
    {
    priv = 999999999;
    }

  if(protect == -1)
    {
    protect = 99999999;
    }

  if(pub>priv || pub>protect)
     {
     if(pub>=priv && pub <=protect)
       {
       before = priv;
       after = protect;
       }
     else if(pub<=priv && pub >= protect)
       {
       before = protect;
       after = priv;
       }
     else
       {
       before = pub;
       after = pub;
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
}


/** Find protected area in source code. */
void Parser::FindProtectedArea(long &before, long &after, size_t startPos) const
{
  before = 0;
  after = 0;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public", startPos);
  long priv = m_BufferNoComment.find("private", startPos);
  long protect = m_BufferNoComment.find("protected", startPos);

  if(priv == -1)
    {
    priv = 999999999;
    }

  if(pub == -1)
    {
    pub = 99999999;
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
       before = pub;
       after = priv;
       }
     else
       {
       before = protect;
       after = protect; // end of class
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
}

/** Find private area in source code. */
void Parser::FindPrivateArea(long &before, long &after, size_t startPos) const
{
  before = 0;
  after = 0;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public", startPos);
  long priv = m_BufferNoComment.find("private", startPos);
  long protect = m_BufferNoComment.find("protected", startPos);

  if(pub == -1)
    {
    pub = 999999999;
    }

  if(protect == -1)
    {
    protect = 99999999;
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
       before = protect;
       after = pub;
       }
     else
       {
       before = priv;
       after = priv; // end of class
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
}

//----------------------------------------------------------------------------------
/** Find the maximum setInput method. In some classes it helps to detect
    the correct number of inputs. */
//----------------------------------------------------------------------------------
/*int Parser::FindSetInputNum(const std::string & buffer, size_t startPos) const
{
  // Return value.
  int numInputs=0;

  // Variables to restrict the search to the public part only
  long before = 0;
  long after = 0;

  FindPublicArea(buffer, before, after, startPos);

  // Find all SetInput methods which have something with "Image" 
  // or "LevelSet" in its parameters.
  long first = buffer.find("SetInput1", before);
  long img   = buffer.find("Image", before);
  long last  = buffer.find(";", first);
  if ((last>first) && (numInputs<1) && (img < last)){ numInputs = 1; }

  // Search for SetInput2 in the same way.
  first = buffer.find("SetInput2", before);
  img   = buffer.find("Image", before);
  last  = buffer.find(";", first);
  if ((last>first) && (numInputs<2) && (img < last)){ numInputs = 2; }

  // Search for SetInput3 in the same way.
  first = buffer.find("SetInput3", before);
  img   = buffer.find("Image", before);
  last  = buffer.find(";", first);
  if ((last>first) && (numInputs<3) && (img < last)){ numInputs = 3; }

  return numInputs;
}
*/

/** Find Set/Get Macro Parameters which are public*/
/*void Parser::FindAndAddParameters(std::string buffer, XMLDescription &desc, size_t startPos) const
{
  // Chck for the following set macros:
  static const int NumMacros=5;
  static const std::string MacroNames[NumMacros][2] =
  {
    { "itkSetMacro",            ")" }, 
    { "itkSetStringMacro",      ")" },
    { "itkSetObjectMacro",      ")" },
    { "itkSetConstObjectMacro", ")" },
    { "itkSetClampMacro",       "," }
  };

  // Variables to restrict the search to the public part only
  long before = 0;
  long after = 0;

  FindPublicArea(buffer, before, after, startPos);

  // Look for each macro set string.
  for (int m=0; m < NumMacros; ++m)
    {
    long first = buffer.find(MacroNames[m][0], before);
    long last = buffer.find(";",first);

    while((last>first) && (first!= -1) && (last !=-1) && (last<after))
      {
      ParametersDescription param;
      std::string val = buffer.substr(first,last-first);

      // Find the Name after "(":
      long p0 = val.find("(",0);
      long p1 = val.find(",",p0);
      std::string Name = val.substr(p0+1,p1-p0-1);
      RemoveChar(Name,' ');

      // Find the Type if it's not a string set macro. We need to search 
      // from the first comma to the next separator symbol. For macros 
      // with more than two params it's the next ",", otherwise a ")".
      std::string paramType = "";
      if (MacroNames[m][0] == "itkSetStringMacro")
        {
        paramType = "String";
        }
      else
        {
        p0 = val.find(",",0);
        p1 = val.find(MacroNames[m][1] ,p0+1);
        paramType = val.substr(p0+1,p1-p0-1);
        RemoveChar(paramType, ' ');
        }

      if (!paramType.empty()) 
        {
        param.call = "Set";
        param.call += Name;
        param.name = Name;
        param.type = paramType;

        desc.parameters.push_back(param);
        }

      // Look for the different set macros.
      first = buffer.find(MacroNames[m][0], last);
      last = buffer.find(";",first);
      }
    }


  // Now find the vector macro ( 3 arguments
  long first = buffer.find("itkSetVectorMacro",before);
  long last = buffer.find(";",first);
  
  while((last>first) && (first!= -1) && (last !=-1) && (last<after))
    {
    ParametersDescription param;
    std::string val = buffer.substr(first,last-first);

    // Find the Name
    long p0 = val.find("(",0);
    long p1 = val.find(",",p0);
    std::string Name = val.substr(p0+1,p1-p0-1);
    RemoveChar(Name,' ');

    // Find the RealType
    p0 = val.find(",",p1);
    p1 = val.find(",",p0+1);
    std::string RealType = val.substr(p0+1,p1-p0-1);
    RemoveChar(RealType,' ');
  
    // Find the dimension
    p0 = val.find(",",p1);
    p1 = val.find(")",p0+1);
    std::string Dimension = val.substr(p0+1,p1-p0-1);
    RemoveChar(Dimension,' ');
  
    std::string paramType = "itk::Vector<";
    paramType += RealType;
    paramType += ",";
    paramType += Dimension;
    paramType += ">";

    param.call = "Set";
    param.call += Name;
    param.name = Name;
    param.type = paramType;

    desc.parameters.push_back(param);

    first = buffer.find("itkSetVectorMacro",last);
    last = buffer.find(";",first);
    }
}
*/


/** Find the typedefs (only the public ones) */
/*void Parser::FindAndAddTypeDefinitions(const std::string & buffer, XMLDescription &desc, size_t startPos) const
{
  // Variables to restrict the search to the public part only
  long before = 0;
  long after = 0;

  FindPublicArea(buffer, before, after, startPos);

  // Now we search the typedefs
  long first = buffer.find("typedef",before);
  if(first == -1)
    {
    return;
    }

  long last = buffer.find(";",first);
  if(last == -1)
    {
    return;
    }

  while((last>first) && (first!= -1) && (last !=-1) && (last<after))
    {
    TypeDefinitions type;
    std::string val = buffer.substr(first,last-first);
 
    // Remove the typedef
    val = val.erase(0,8);

    // If typename exists remove it
    long pos = val.find("typename",0);
    if(pos != -1 && pos < 12)
      {
      val = val.erase(0,pos+8);
      }

    // We remove the "\n" chars
    pos = val.find("\n",0);
    while(pos != -1)  
      {
      if(pos == 0)
        {
        val.erase(pos,1);
        }
      else
        {
        val.erase(pos-1,2);
        }
      pos = val.find("\n",0);
      }

    // we remove any extra spaces before the name
    while(val.find(" ",0) == 0)
      {
      val = val.erase(0,1);
      }

    // if we have something like itk::Image<const char,3>
    // we just have to be sure that the space is not in between brackets
    // Then we find the next space
    pos = val.find(" ",0);
    // we check if the space is not between brackets
    long b1 = val.find("<",0);

    // Find the last > of the typedefs
    long b3 = val.find(">",0);
    long b2 = -1;
    while(b3!=-1)
      {
      b2 = b3;
      b3 = val.find(">",b3+1);
      }

    while(pos>b1 && pos<b2 && pos!= -1 && b1!=-1)
      {
      pos = val.find(" ",pos+1);
      }

    if(pos == -1)
      {
      return;
      }

     std::string Reference = val.substr(0,pos);
 
    // We remove extra spaces, by having only one space between words
    RemoveChar(Reference,' ');
    long spacepos = Reference.find("  ");
    while(spacepos != -1)
      {
      Reference.erase(spacepos,1);
      spacepos = Reference.find("  ",0);
      }

    type.reference = Reference; 

    if(pos == -1)
      {
      return;
      }

    std::string Name = val.substr(pos,val.size()-pos);
    RemoveChar(Name,' ');
    type.name = Name;
    desc.typedefs.push_back(type);
    first = buffer.find("typedef",last);
    if(first == -1)
      {
      return;
      }
    last = buffer.find(";",first);
    if(first == -1)
      {
      return;
      }
    }
}
*/


/**  return true if the position pos is inside a comment */
bool Parser::IsInComments(long int pos) const
{
  if((pos == -1) || (m_CommentBegin.size()==0) || (m_CommentEnd.size() == 0))
    {
    return false;
    }

  long int b0 = m_Buffer.find(m_CommentBegin,0);
  long int b1 = m_Buffer.find(m_CommentEnd,b0);

  while(b0 != -1 && b1 != -1 && b1>b0)
    {
    if(pos>=b0 && pos<=(b1+(long int)m_CommentEnd.size()))
      {
      return true;
      }
    b0 = m_Buffer.find(m_CommentBegin,b0+1);
    b1 = m_Buffer.find(m_CommentEnd,b0);
    }

  return false;
}

/**  return true if the position pos is between 'begin' and 'end' */
bool Parser::IsBetweenChars(const char begin, const char end ,long int pos) const
{
  if(pos == -1)
    {
    return false;
    }

  long int b0 = m_BufferNoComment.find(begin,0);
  long int b1 = m_BufferNoComment.find(end,b0);

  while(b0 != -1 && b1 != -1 && b1>b0)
    {
    if(pos>b0 && pos<b1)
      {
      return true;
      }
    b0 = m_BufferNoComment.find(begin,b0+1);
    b1 = m_BufferNoComment.find(end,b0);
    }

  return false;
}

/** Return true if the class has a template */
long int Parser::IsTemplated(const std::string & buffer, long int classnamepos) const
{
  long int templatepos = buffer.find("template",0);
  long int returnval = templatepos;

  while(templatepos!=-1 && templatepos<classnamepos)
    {
    // find the last > that corresponding to the template
    long int p0 = buffer.find("<",templatepos);
    long int pos = -1;
    if(p0!=-1)
      {
      long int i=p0+1;
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

    if(pos<classnamepos && pos!=-1)
      {
      bool ok = true;
      // Check if between the last > and classnamepos we don't have words
      for(int i=pos+1;i<classnamepos;i++)
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


/** Find the name of the filter and templates, etc ... Returns the position 
    within the file where class definition starts. */
/*size_t Parser::FindAndAddName(const std::string & buffer, XMLDescription &desc, std::string fileName) const
{
  // Position where the class code starts. We need that position later to 
  // avoid that parameters etc. are found in other classes before.
  size_t retPos=0;

  long first = buffer.find("class ",0);
  long last = buffer.find("{",first);

  // Create a temporary vector of MLDescription to store all the possible classes
  std::vector<XMLDescription> descVector;
  std::vector<size_t> startPosVector;

  std::string Name = "";

  // Sometimes there is something like:
  // template class Image<unsigned short,3>; before the real template

  long semicolon= buffer.find(";",first);

  while((semicolon < last) && (semicolon != -1))
    {
    first = buffer.find("class ",first+1);
    semicolon= buffer.find(";",first);
    }


  while(last>first && first!=-1)
    {
    // Search if the keyword class is within <>
    // Count the number of < and > before the name
    // We also have to restrict the search between the word 'template' and 'class'
    // First we find the word 'template' closest to our keyword 'class'
    long b = buffer.find("template",0);
    while(b!=-1 && b<first)
      {
      long b0 = buffer.find("template",b+1); 
      if(b0 < first && b0!= -1)
        {
        b = b0;
        }
      else
        {
        break;
        }
      }

    bool inside = false;

    if(b != -1)
      {
      long b0 = buffer.find("<",b);
      long b1 = buffer.find(">",b0);

      unsigned int inf = 0;
      unsigned int sup = 0;

      while(b0<=first && b0 != -1)
        {
        inf++;
        b0 = buffer.find("<",b0+1);
        }

       while(b1<=first && b1 != -1)
        {
        sup++;
        b1 = buffer.find(">",b1+1);
        }

       if(inf > sup)
         {
         inside = true;
         }
      }

    if(!inside) // This is a valid class
      {

      // Create a temporary description to be added to the descVector;
      XMLDescription descTemp;
      descTemp.name = "";

    // Extract the name of the class
    std::string val = buffer.substr(first,last-first);

    // Are we a derived class?
    std::string Derived = "";
    long pos = val.find("public",0);
    if(pos != -1) // if yes we not the class name (only one class for the moment)
      {
      Derived = val.substr(pos+7,val.size()-pos-7);
      RemoveChar(Derived,' ');
      val = val.substr(0,pos);
      }

    // Now we remove unwanted keywords: class, ITK_EXPORT
    pos = val.find("class",0);
    if(pos != -1)
      {
      val = val.erase(pos,5);
      }
    pos = val.find("ITK_EXPORT",0);
    if(pos != -1)
      {
      val = val.erase(pos,10);
      }
    pos = val.find(":",0);
    if(pos != -1)
      {
      val = val.erase(pos,1);
      }
    pos = val.find("\n",0);
    if(pos != -1)
      {
      val = val.erase(pos-1,2);
      }
    
    Name = val;
    RemoveChar(Name,' ');  
    descTemp.name = Name;
    
    pos = Derived.find("\n",0);
    
    // Remove any \n
    while(pos != -1)
      {
      if(pos == 0)
        {
        Derived = Derived.erase(pos,1);
        }
      else
        {
        Derived = Derived.erase(pos-1,2);
        }
      pos = Derived.find("\n",pos);
      }

  
    // Remove any spaces
    pos = Derived.find(" ",0);
    while(pos != -1)
      {
      Derived = Derived.erase(pos,1);
      pos = Derived.find(" ",pos);
      }
 
    descTemp.superclass = Derived;

    //std::cout << "Class name = " << descTemp.name.c_str() << std::endl;

    // Search if the class has template parameters
    if(this->IsTemplated(buffer,first) != -1 && descTemp.name.size()>0)
      {
      pos = this->IsTemplated(buffer,first);

      // We extract the template arguments
      val = buffer.substr(pos,first-pos);

      //std::cout << "Full Template = " << val.c_str() << std::endl;

      // we remove the template keyword
      val = val.erase(0,9);
      pos = val.find("\n",0);
      if(pos != -1)
        {
        val = val.erase(pos-1,2);
        }
    
      RemoveChar(val,'<');
      RemoveChar(val,'>');


      while(val.find(",",0) != -1 && (pos != -1))
        {     
       pos = val.find(",",0);

        // We want to be sure that the coma is not between brackets due to default template parameters
        while((pos != -1) && (IsBetweenBrackets(val,pos)) )
          {
          pos = val.find(",",pos+1);
          }
       
        if(pos!= -1)
          {
          std::string arg = val.substr(0,pos);
          RemoveChar(arg,' ');
          RemoveCtrlN(arg);
          RemoveChar(arg,' ');
          
          TemplateArguments templ;
          // Look if we have a default template argument
          if(arg.find("=",0)!= -1)
            {
            // template name
            long int templPos = arg.find("=",0);
            std::string templName = arg.substr(0,templPos);
            RemoveChar(templName,' ');
            templ.name = templName;
            
            // template default value
            std::string templValue = arg.substr(templPos+1,arg.size()-templPos-1);
            RemoveChar(templValue,' ');
            RemoveCtrlN(templValue);
            RemoveChar(templValue,'>');
            RemoveChar(templValue,' ');
            templ.value = templValue;
            }
          else
            {
            templ.name = arg;
            templ.value = "";
            }
          
          descTemp.templateArgs.push_back(templ);
          // Remove the argument
          val = val.erase(0,pos+1);
          }
        }

      std::string arg = val.substr(0,val.size());
      RemoveChar(arg,' ');
      RemoveCtrlN(arg);
      RemoveChar(arg,' ');
      
      TemplateArguments templ;
      // Look if we have a default template argument
      if(arg.find("=",0)!= -1)
        {
        // template name
        long int templPos = arg.find("=",0);
        std::string templName = arg.substr(0,templPos);
        RemoveChar(templName,' ');
        templ.name = templName;
           
        // template default value
        std::string templValue = arg.substr(templPos+1,arg.size()-templPos-1);
        RemoveChar(templValue,' ');
        RemoveChar(templValue,'>');
        RemoveChar(templValue,' ');
        templ.value = templValue;      
        }
      else
        {
        templ.name = arg;
        templ.value = "";
        }    
      descTemp.templateArgs.push_back(templ);
      }


    // Add the temporary description to the vector. Also store 
    // position in file where the class code begins.
    descVector.push_back(descTemp);
    startPosVector.push_back(first);
    }
    first = buffer.find("class ",first+1);
    last = buffer.find("{",first);
   }

  // If we have one class everything is fine we take this one
  if(descVector.size() == 1)
    {
    desc.name = descVector[0].name;
    desc.superclass = descVector[0].superclass;
    desc.templateArgs = descVector[0].templateArgs;
    retPos = startPosVector[0];
    }
  else // we take the class that has the same name as the header file
    {
    // We need a precise comparison of found class with class name derived from file name.
    std::string expectedClassName = fileName;

    // Search and remove path fragments, all inclusive "itk" and then ".h" or ".cpp"
    // to determine the real class name from the file name.
    size_t pos = 0;
    while ((pos = expectedClassName.find("/")) != std::string::npos){ expectedClassName.erase(0, pos+1); }
    while ((pos = expectedClassName.find("\\")) != std::string::npos){ expectedClassName.erase(0, pos+1); }
    pos = expectedClassName.find("itk");
    if (pos != std::string::npos){ expectedClassName.erase(0, pos+3); }
    pos = expectedClassName.find(".h");
    if (pos != std::string::npos){ expectedClassName.erase(pos, 2); }
    pos = expectedClassName.find(".cpp");
    if (pos != std::string::npos){ expectedClassName.erase(pos, 3); }

    for(unsigned int i=0;i<descVector.size();i++)
      {

    if(descVector[i].name.find(expectedClassName.c_str()) != -1)
        {
        desc.name = descVector[i].name;
        desc.superclass = descVector[i].superclass;
        desc.templateArgs = descVector[i].templateArgs;
        return startPosVector[i];
        }
      }

    std::cout << "ERROR: The file " << expectedClassName.c_str() << " doesn't have a class with the same name as the header file!" << std::endl;
    //system("PAUSE");
    }

  // Return start position of class code.
  return retPos;
}
*/

/** Remove the comments */
void Parser::RemoveComments()
{
  m_BufferNoComment = m_Buffer;
  m_CommentPositions.clear();
  unsigned long size = m_BufferNoComment.size();
  unsigned long count = 0;
  // first we find the /* */
  long first = m_BufferNoComment.find("/*",0);
  long last = m_BufferNoComment.find("*/",first);
 
  // We find if there are spaces between */ and the end of the line
  bool extraSpaces = true;
  long eol = m_BufferNoComment.find("\n",last);
  
  if(eol != -1)
    {
    for(int i=last+2;i<eol;i++)
      {
      if(m_BufferNoComment[i] != ' ' && m_BufferNoComment[i] != '\r')
        {
        extraSpaces = false;
        }
      }
    }

  if(extraSpaces && (last>first) && (last!=-1))
    {
    last += (eol-last)-1;
    }

  long int offset = 0;

  while((last>first) && (first!= -1) && (last !=-1))
    {
    PairType pair(first+offset,last+offset);
    m_CommentPositions.push_back(pair);
    offset += last+2-first;
    m_BufferNoComment = m_BufferNoComment.erase(first,last+2-first);
    count += last+2-first;
    first = m_BufferNoComment.find("/*",0);
    last = m_BufferNoComment.find("*/",first);
    // We find if there are spaces between */ and the end of the line
    bool extraSpaces = true;
    long eol = m_BufferNoComment.find("\n",last);
    if(eol != -1)
      {
      for(int i=last+2;i<eol;i++)
        {
        if(m_BufferNoComment[i] != ' ')
          {
          extraSpaces = false;
          }
        }
      }
    if(extraSpaces && (last>first) && (last!=-1))
      {
      last += (eol-last)-1;
      }
    };

  // Then the // comments
  first = m_BufferNoComment.find("//",0);
  last = m_BufferNoComment.find("\n",first);
  
  while((last>first) && (first!= -1) && (last !=-1))
    {
    PairType pair(first+offset,last+offset);
    m_CommentPositions.push_back(pair);
    m_BufferNoComment = m_BufferNoComment.erase(first,last+1-first);
    offset += last+1-first; 
    count += last+1-first;
    first = m_BufferNoComment.find("//",0);
    last = m_BufferNoComment.find("\n",first);
    };


  // put a 0 char at the end of the buffer so we are sure to have the correct length. 
  // This is tricky
  m_BufferNoComment[size-count] = '\0';
  m_BufferNoComment.resize(size-count);
}


/** Find the constructor in the file */
long Parser::FindConstructor(const std::string & buffer, const std::string & className, bool headerfile, size_t startPos) const
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
  long pos = buffer.find(constructor.c_str(),startPos);
  long pos2 = buffer.find("(",pos);
  long pos3 = buffer.find(")",pos2);
  
  // if it's not the destructor ...
  /*std::string dest = "~";
  dest += className;
  long destructor = buffer.find(dest.c_str(),0);*/

  while(pos != -1 && pos2 != -1 && pos3 != -1)
    {
    //if(pos != destructor+1) // we have the constructor
      {
      // we look if we have only spaces or \n between pos pos2 and pos3
      std::string val = buffer.substr(pos+className.size()+1,pos2-pos-className.size()-1);
      bool ok = true;
      for(unsigned int i=0;i<val.size();i++)
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
        for(unsigned int i=0;i<val.size();i++)
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
        long pos4 = buffer.find(";",pos3);
        long pos5 = buffer.find("{",pos3);

        if(pos5<pos4)
          {
          return pos5;
          }
        else
          {
          return -1;
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

  return -1;
}


/** Starting from startPos scan for all SetFunctions which are not implemented 
    by a macro. Add their type and name to the list of parameters.
*/
/*void Parser::FindAndAddNonMacroSetFunctions(std::string buffer, XMLDescription &desc, long startPos) const
{
  // Variables to restrict the search to the public part only
  long before = 0;
  long after = 0;
  FindPublicArea(buffer, before, after, startPos);

  // Find all SetFunctions if there are some.
  std::string paramName = ""; 
  std::string paramType = "";
  size_t found = startPos;
  do{
    bool okay = false;
    bool isVoidSetFunc = false;

    if (desc.name.find("EuclideanDistanceImage") ==0){
      okay = true;
      okay = false;
    }

    found = FindAndAddNonMacroSetFunction(buffer, found, paramName, paramType, okay, isVoidSetFunc);
    if ((okay || isVoidSetFunc) && (found > (size_t)before) && (found <= (size_t)after))
      {
      // Check whether the parameter already exists. If yes then do not add.
      bool alreadyExists = false;
      for (size_t n=0; !alreadyExists && (n < desc.parameters.size()); ++n)
        {
        // Name identical? If yes then do not add it.
        alreadyExists |= (paramName == desc.parameters[n].name);
        }
      if (!alreadyExists)
        {
        // Yes, valid function found.
        ParametersDescription param;

        param.call = "Set";
        param.call += paramName;
        param.name = paramName;
        param.type = isVoidSetFunc ? "void" : paramType;

        //std::cout << "***********************  Non macro Set-Function " << paramName.c_str() << 
        //             " found for class " << desc.name.c_str() << "." << std::endl;

        desc.parameters.push_back(param);
        }
      }
    }
  while (std::string::npos != found);
  
}
*/

/** Find first occurrence of a SetFunction which is not implemented by a macro. 
    Return position after function ")" if found or any position after "void" if not found.
    flag = true indicates successful finding, otherwise false.
    isVoidSetFunc is returned as true if a function like "void Set...()" or
    "void Set...(void)" is detected.
 */
/*size_t Parser::FindAndAddNonMacroSetFunction(std::string buffer,
                                                         long startPos,
                                                         std::string &paramName, 
                                                         std::string &paramType,
                                                         bool &flag,
                                                         bool &isVoidSetFunc) const
{

  // Assume a valid set function.
  flag = true;

  // Be sure that all newlines are replaced be single spaces.
  size_t pos = 0;
  do {
    pos = buffer.find('\n', startPos);
    if (std::string::npos != pos){ buffer = buffer.replace(pos, 1, " "); }
  } while (std::string::npos != pos);

  // Also be sure that all carriage returns are replaced by single spaces.
  pos = 0;
  do {
    pos = buffer.find('\r', startPos);
    if (std::string::npos != pos){ buffer = buffer.replace(pos, 1, " "); }
  } while (std::string::npos != pos);

  const size_t fVoidBeforeStart = buffer.find("void", startPos);
  size_t fStart                 = buffer.find("Set", fVoidBeforeStart);
  size_t fParOpen               = buffer.find("(", fStart);
  const size_t fOrigParClose    = buffer.find(")", fParOpen);

  if ((std::string::npos != fVoidBeforeStart) &&
      (std::string::npos != fStart) &&
      (std::string::npos != fParOpen) &&
      (std::string::npos != fOrigParClose) &&
      (fOrigParClose > fParOpen) &&
      (fStart > fVoidBeforeStart) &&
      (fParOpen > fStart))
    {
    // Get string between "void" and "Set". There must be only spaces.
    std::string spaces = buffer.substr(fVoidBeforeStart+4, fStart-(fVoidBeforeStart+4));
    for (size_t scnt=0; scnt<spaces.size(); ++scnt)
      {
      if (spaces[scnt] != ' ')
        {
        // On failure return position after fVoidBeforeStart; maybe more luck on next check.
        flag = false;
        return fVoidBeforeStart+1;
        }
      
      }

    if (flag)
      {
      // Get entire function as string.
      buffer = buffer.substr(fStart, fOrigParClose-fStart+1);

      // Remove "void " at start.
      do {
        pos = buffer.find("void ");
        if (pos != std::string::npos){ buffer = buffer.replace(pos, 5, ""); }
      } while (std::string::npos != pos);

      // Remove eventual spaces before "(".
      do {
        pos = buffer.find(" (");
        if (pos != std::string::npos){ buffer = buffer.replace(pos, 1, ""); }
        // Correct position of open parentheses.
        fParOpen--;
      } while (std::string::npos != pos);

      // Remove eventual spaces after "(".
      do {
        pos = buffer.find("( ");
        if (pos != std::string::npos){ buffer = buffer.replace(pos+1, 1, ""); }
      } while (std::string::npos != pos);

      // Remove eventual spaces before ")".
      do {
        pos = buffer.find(" )");
        if (pos != std::string::npos){ buffer = buffer.replace(pos, 1, ""); }
      } while (std::string::npos != pos);

      // Remove all "const " from function parameters.
      do {
        pos = buffer.find("const ");
        if (pos != std::string::npos){ buffer = buffer.replace(pos, 6, ""); }
      } while (std::string::npos != pos);

      // Remove all "typename " from function parameters.
      do {
        pos = buffer.find("typename ");
        if (pos != std::string::npos){ buffer = buffer.replace(pos, 9, ""); }
      } while (std::string::npos != pos);

      // Search for commas. We still cannot handle multiple parameter functions.
      pos = buffer.find(",");
      if (std::string::npos != pos)
        {
        flag = false;
        }
      }

    }
  else
    {
     // No set function found any more. Return npos to terminate search.
     flag = false;
     return std::string::npos;
    }

  if (flag)
    {
    // Seems to be okay.
    // Correct values for fStart, fParOpen and fParClose since string has changed.
    fStart           = 0;
    fParOpen         = buffer.find("(", fStart);

    // Look for array set functions. We still cannot handle it.
    pos = buffer.find("[");
    if (std::string::npos != pos)
      {
       flag = false;
       return fVoidBeforeStart+1;
      }

    // Search for last "&" in params. If yes then remove 
    // everything between & and ")" to have only the type.
    pos = buffer.rfind("&");
    if (std::string::npos != pos)
      {
      // Erase rest inclusive "&" to leave type alone at end.
      buffer.erase(pos);
      }
    else 
      {
      // Search last "*". 
      pos = buffer.rfind("*");
      if (std::string::npos != pos)
        {
        // Erase rest to leave type alone at end. Do not remove "*".
        buffer.erase(pos+1);
        }
      else 
        {
        // Remove all spaces directly after "(".
        while ((fParOpen < buffer.size()) &&
               (' ' == buffer[fParOpen+1])){ buffer.erase(fParOpen+1); }

        // Look for first space after "(".
        pos = buffer.find(" ", fParOpen);
        if (std::string::npos != pos)
          {
          // Erase rest to leave type alone at end. Also remove " ".
          buffer.erase(pos);
          }
        else
          {
          // Error, type could not be extracted.
          flag = false;

          // However, it could be a Set-function without
          // parameters. It could be generated as Notify button.
          // Look for ")".
          size_t fParClose = buffer.find(")", fParOpen);
          if ((fParClose != std::string::npos) && (fParClose > fParOpen))
            {
            if (fParClose == fParOpen+1)
              {
              // Something like "void Set...()"
              isVoidSetFunc = true;
              }
              else
              {
              // Something like "void Set...(void)"
              if (buffer.substr(fParOpen+1, fParClose-(fParOpen+1)) == "void")
                {
                isVoidSetFunc = true;
                }
              }

              // If a void set function has been found then return correct positions.
              if (isVoidSetFunc)
                {
                paramName = buffer.substr(fStart+3, fParOpen-(fStart+3));
                paramType = "";
                return fOrigParClose+1;
                }
            }
          }
        }
      }
   }

   if (flag)
     {
     // Extract param name from function name (pos after "Set" until "(").
     paramName = buffer.substr(fStart+3, fParOpen-(fStart+3));
     paramType = buffer.substr(fParOpen+1);

     // Suppress some parameter types handled differently or not handled.
     if ((paramType.find("FunctorType")  != std::string::npos) || 
         (paramType.find("ImageType")    != std::string::npos) || 
         (paramType.find("KernelType")   != std::string::npos) || 
         (paramType.find("FunctionType") != std::string::npos) || 
         (paramType.find("CostFunction") != std::string::npos) || 
         (paramType.find("TInputImage")  != std::string::npos) || 
         (paramName == "Input")  || 
         (paramName == "Input1") || 
         (paramName == "Input2") || 
         (paramName == "Input3"))
       {
       flag = false;
       }

     // Both are not empty for some reasons?
     flag &= !paramName.empty();
     flag &= !paramType.empty();
     }

   // Return npos on error or position after closed parentheses.
   return flag ? fOrigParClose+1 : fVoidBeforeStart+1;
}
*/

/** Find the defaults var values in the .txx or .cxx file */
/*void Parser::FindAndAddDefaultValues(std::string buffer, long start, XMLDescription &desc) const
{
  // For each parameters we look at the value
  for(unsigned int i=0;i<desc.parameters.size();i++)
    {

    //if (desc.name.find("Euler2DTransform")!= std::string::npos){
    //  i=i;
    //}

    // Find parameter followed by "." or with " " or with "=". Also permit "["
    // although that's sometime problematic since assignments of scalars to vectors 
    // not always might be legal.
    // Otherwise we might find "m_BandwidthValue = " instead of "m_Bandwidth =".
    // We assume that the constructor is first in the file.
    long pos = buffer.find(desc.parameters[i].name+"=",start);
    if (pos == -1)
      {
       pos = buffer.find(desc.parameters[i].name+" =",start);
      }
    if (pos == -1)
      {
       pos = buffer.find(desc.parameters[i].name+".",start);
      }
    if (pos == -1)
      {
       pos = buffer.find(desc.parameters[i].name+" .",start);
      }
    if (pos == -1)
      {
       pos = buffer.find(desc.parameters[i].name+"[",start);
      }
    if (pos == -1)
      {
       pos = buffer.find(desc.parameters[i].name+" [",start);
      }

    // Look for the ";" which terminates the initialization statement.
    long pos2  = buffer.find(";",pos);

    // Look for terminating "{" it should not be before ";".
    long pos3  = buffer.find("{",pos);
    if ((pos3!=-1) && (pos3 < pos2))
      {
      // If there is a "{" before ";" then it seems to be an illegal initialization.
      pos = -1;
      }

    // Look for terminating "}" it should not be before ";".
    long pos4  = buffer.find("}",pos);
    if ((pos4!=-1) && (pos4 < pos2))
      {
      // If there is a "}" before ";" then it seems to be an illegal initialization.
      pos = -1;
      }

    if(pos!= -1 && pos2 != -1)
      {
      std::string val = buffer.substr(pos,pos2-pos).c_str();

      // assignment are = and Fill
      // if =
      long pos3 = val.find("=",0);
      if ((pos3 != -1))
        {
        val = val.substr(pos3+1,pos2-pos3-1);
        RemoveChar(val,' ');

        // Remove leading space.
        while ((val.size() > 0) && (val[0]==' '))
          {
           val.erase(0,1);
          }

        // Check for invalid values, for example single non numeric variables or function 
        // calls, which cause too much trouble.
        bool invalid = false;
        if (val.find("->") != std::string::npos)
          {
          invalid = true;
          }
        if ((val.size() == 1) && ((val[0] < '0') || (val[0] > '9')))
          {
          invalid = true;
          }

        // Check for numeric values or for something like "::" in 
        // it which indicates constants.
        float fVar=0;
        if ((val.size() > 0) && (0==sscanf(val.c_str(), "%f", &fVar)))
          {
          // No number. If there is no colon in it we assume that it is 
          // a loop var or an unknown constant.
          if (val.find("::") == std::string::npos)
            {
            invalid = true;
            }
          }

        // Check for some other hints for valid values.
        if ((val.find("NULL") != std::string::npos) ||
            (val.find("false") != std::string::npos) ||
            (val.find("true") != std::string::npos) ||
            (val.find("ZeroOrder") != std::string::npos) ||
            (val.find("ImageDimension") != std::string::npos) ||
            (val.find("UNINITIALIZED") != std::string::npos) ||
            (val.find("static_cast") != std::string::npos) ||
            (val.find("m_Threader->GetNumberOfThreads()") != std::string::npos) ||
            (val.find("BoundValueType") != std::string::npos) ||
            (val.find("BoundSelectionType") != std::string::npos) ||
            (val.find("NumericTraits") != std::string::npos) ||
            (val.find("INITIALIZED") != std::string::npos))
          {
          invalid = false;
          }

        // This causes some problems since its used outside templates.
        if (val.find("TRealType") != std::string::npos)
          {
          invalid = true;
          }


        if (!invalid){ desc.parameters[i].defaultvalue = val; }
        }
      pos3 = val.find("Fill(",0);
      if (pos3 != -1)
        {
        long pos4= val.find(")",pos3);
        val = val.substr(pos3+5,pos4-pos3-5);
        RemoveChar(val,' ');
        desc.parameters[i].defaultvalue = val;
        desc.parameters[i].fillDefaultVal="Fill";
        }
      }

    }
}
*/

/** Find the closing char given the position of the opening char */
long int Parser::FindClosingChar(char openChar, char closeChar, long int pos) const
{
  long int open = 1;
  for(size_t i=pos+1;i<m_Buffer.length();i++)
    { 
    if(m_Buffer.c_str()[i] == openChar)
      {
      open++;
      }
    else if(m_Buffer.c_str()[i] == closeChar)
      {
      open--;
      }
    if(open == 0)
      {
      return (long int)i;
      }
    }
  return -1; // closing bracket not found
}

/** Check if the name of the class 
 *  The class name is the one that is not between anything and does not have ;
 *  before the } */
bool Parser::CheckNameOfClass(const char* name,const char* prefix)
{
  bool gotMatch = false;
  long int pos = m_BufferNoComment.find("class",0);
  long int errorpos = 0;
  std::string nameOfClass = "";
  while(pos!=-1)
    {
    if(!IsBetweenChars('<','>',pos))
      {
      bool valid = true;
      // We should get a { before a ;
      long int i = pos+4;
      while((m_BufferNoComment[i] != '{')
         && (i<(long)m_BufferNoComment.size())
         )
        {
        if(m_BufferNoComment[i] == ';')
          {
          valid = false;
          break;
          }
        else if(m_BufferNoComment[i] == ':')
          {
          break;
          }
        i++;
        }

      if(valid)
        {
        errorpos = i;
        nameOfClass = this->FindPreviousWord(i);

        if(m_Filename == "")
          {
          std::cout << "CheckIfNDefDefine() : m_Filename shoud be set" << std::endl;
          return false;
          }

        long int point = m_Filename.find_last_of(".");
        long int slash = m_Filename.find_last_of("/");

        if(slash == -1)
          {
          slash = 0;
          }

        std::string nameofclass = m_Filename.substr(slash+1,point-slash-1);
        std::string extension = m_Filename.substr(point+1,m_Filename.size()-point-1);

        // construct the string
        std::string toMatch = name;
        long int p = toMatch.find("<NameOfClass>");
        if(p != -1)
          {
          toMatch.replace(p,13,nameofclass);
          }
        p = toMatch.find("<Extension>");
        if(p != -1)
          {
          toMatch.replace(p,11,extension);
          }

        nameOfClass = prefix+nameOfClass;

        if(nameOfClass == toMatch)
          {
          gotMatch = true;
          break;
          }
        }
      }
    pos = m_BufferNoComment.find("class",pos+1);
    }

  if(!gotMatch)
    {
    Error error;
    error.line = this->GetLineNumber(errorpos,true);
    error.line2 = error.line;
    error.number = NAMEOFCLASS;
    error.description = "classname is not defined correctly";
    m_ErrorList.push_back(error);
    return false;
    }

  return true;
}

/** Find the name space 
 *  using namespace is prohibited for this parser 
 *  We look if the closing bracket is at the end of the file, if yes we accept otherwise we reject*/
bool Parser::CheckNamespace(const char* name)
{
  std::string nameSpace = "";

  long int pos = m_BufferNoComment.find("namespace",0);

  if(pos == -1)
    {
    Error error;
    error.line = this->GetLineNumber(0,true);
    error.line2 = error.line;
    error.number = NAMESPACE;
    error.description = "namespace not defined";
    m_ErrorList.push_back(error);
    return false;
    }

  
  // check that the word is not using namespace
  bool ok = true;
  if(this->FindPreviousWord(pos) == "using")
    {
    ok = false;
    }

  if(ok)
    {
    std::string names = name;
    // extract the namespace
    std::string nspace = this->FindNextWord(pos);
    if(nspace != names)
      {
      std::cout << "-" << nspace.c_str() << "--" << names.c_str() << "-" << std::endl;
      Error error;
      error.line = this->GetLineNumber(pos,true);
      error.line2 = error.line;
      error.number = NAMESPACE;
      error.description = "namespace is wrong";
      m_ErrorList.push_back(error);
      return false;
      }
    }

  /*
  // Now search if we have other namespaces
  while(pos!= -1 && ok)
    {
    pos = buffer.find("namespace",pos+1);
    // check that the word is not using namespace
    use = buffer.find("using",pos-20);

    ok = true;

    if((use!=-1 && pos-use<20) || (pos == -1))
      {
      ok = false;
      }

    if(ok)
      {
      // extract the namespace
      long int bracket = buffer.find("{",pos);
      long int closing = FindClosingBracket(buffer,bracket);

      // Find the number of } after closing
      int num = 0;
      int close = closing+1;
      while(buffer.find("}",close) != -1)
        {
        num++;
        close=buffer.find("}",close);
        close++;
        }

      if(bracket != -1 && bracket-pos-10<10 && num==nNameSpace)
        {
        std::string name = buffer.substr(pos+10,bracket-pos-10);
        RemoveCtrlN(name);
        RemoveChar(name,' ');
        RemoveCtrlN(name);
        nameSpace += "::";
        nameSpace += name;
        nNameSpace++;
        }
      }
    }
    */

  return true;
}

/** Remove string area from start to end (incl) from buffer if filename contains className. */
std::string Parser::RemoveArea(const std::string fileName,
                                           std::string buffer, 
                                           const std::string className,
                                           const std::string &start, const std::string &end)
{
  if (fileName.find(className) != std::string::npos)
    {
    size_t pos1 = buffer.find(start);
    size_t pos2 = buffer.find(end, pos1);
    if ((std::string::npos != pos1) &&
        (std::string::npos != pos2) &&
        (pos1 < pos2))
      {
      // Remove area if found.
      buffer.erase(pos1, (pos2+end.size())-pos1);
      }
    }

  return buffer;
}

/** Check the comments
 * The comment definition should be set before CheckIndent() to get the correct indentation
 * for the comments. */
bool Parser::CheckComments(const char* begin,const char* middle,const char* end)
{
  bool hasError = false;
  // Set the ivars for the indent checking
  m_CommentBegin = begin;
  m_CommentMiddle = middle;
  m_CommentEnd = end;

  return !hasError;
}


} // end namespace kws
