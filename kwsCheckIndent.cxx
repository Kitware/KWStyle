/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckIndent.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

#define ALIGN_LEFT -99999

/** Extract the current line from pos to  \n */
std::string Parser::ExtractLine(long pos)
{
  long int p = m_Buffer.find("\n",pos);
  if(p>pos)
    {
    return m_Buffer.substr(pos,p-pos-1);
    }
  return ""; 
}

/** Return the current ident */
long int Parser::GetCurrentIdent(std::string line,char type)
{
  long int indent = 0;
  std::string::const_iterator it = line.begin();
  while(it != line.end() && (*it)== type)
    {
    indent++;
    it++;
    }
  return indent;
}

/** Check the indent size */
bool Parser::CheckIndent(IndentType itype,
                         unsigned long size,
                         bool doNotCheckHeader,
                         bool allowBlockLine,
                         unsigned int maxLength)
{
  m_TestsDone[INDENT] = true;
  m_TestsDescription[INDENT] = "The Indent should respect: ";
  char* val = new char[10];
  sprintf(val,"%d ",size);
  m_TestsDescription[INDENT] += val;
  if(itype == (IndentType)TABS)
    {
    m_TestsDescription[INDENT] += "tabs";
    }
  else
    {
    m_TestsDescription[INDENT] += "spaces"; 
    }
  delete [] val;

  if(doNotCheckHeader)
    {
    m_TestsDescription[INDENT] += " (not checking header, "; 
    }
  else
    {
    m_TestsDescription[INDENT] += " (checking header, ";
    }

  if(allowBlockLine)
    {
    m_TestsDescription[INDENT] += "blockline allowed)"; 
    }
  else
    {
    m_TestsDescription[INDENT] += "blockline not allowed)";
    }
  
  bool hasError = false;
  unsigned long pos = 0;
  unsigned int currentPosition = 0;
  std::string::const_iterator it = m_Buffer.begin();

  // Create the indentation
  this->InitIndentation();

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

  char type = ' ';
  if(itype == (IndentType)TABS) {type = '\t';}

  int wantedIndent = 0;
  
  // We extract the firt line and compute the number of spaces/tabs at the beginning
  std::string line = this->ExtractLine(pos);
  int currentIndent = this->GetCurrentIdent(line,type);
  bool firstChar = true;

  // We start to check
  while(it != m_Buffer.end())
    {
    if((*it) == type || (*it)=='\r')
      {
      it++;
      pos++;
      continue;
      }

    if((*it) == '\n')
      {
      it++;
      pos++;
      // We extract the next line
      std::string line = this->ExtractLine(pos);
      currentIndent = this->GetCurrentIdent(line,type);
      firstChar = true;
      continue;
      }

    // Check if pos is in the list of positions
    std::vector<IndentPosition>::iterator itIdentPos = m_IdentPositionVector.begin();
    IndentPosition* sindent = NULL;
    while(itIdentPos != m_IdentPositionVector.end())
      {
      if((*itIdentPos).position == pos)
        {
        sindent = &(*itIdentPos);
        break;
        }
      itIdentPos++;
      }

    // We check if we have the right indent
    if(sindent)
      {
      long int wanted = wantedIndent+size*sindent->current;
      if(sindent->current == ALIGN_LEFT)
        {
        wanted = 0;
        }

      else if(currentIndent != wanted)
        {
        // We check that the previous line is not ending with a semicolon
        // and that the sum of the two lines is more than maxLength
        std::string previousLine = this->GetLine(this->GetLineNumber(pos)-2);
        std::string currentLine = this->GetLine(this->GetLineNumber(pos)-1);
        if( (previousLine[previousLine.size()-1] != ';')
           && (previousLine.size()+currentLine.size()-currentIndent>maxLength)
          )
          {
          // Do nothing
          }
        else
          {
          Error error;
          error.line = this->GetLineNumber(pos);
          error.line2 = error.line;
          error.number = INDENT;
          error.description = "Special Indent is wrong ";
          char* val = new char[10];
          sprintf(val,"%d",sindent->current); 
          error.description += val;
          error.description += " (should be ";
          delete [] val;
          val = new char[10];
          sprintf(val,"%d",wanted);
          error.description += val;
          error.description += ")";
          delete [] val;
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      wantedIndent += size*sindent->after;
      firstChar = false;
      }
    else if((it != m_Buffer.end()) && ((*it) == '{') && !this->IsInComments(pos)) // openning bracket
      {
      bool check = true;
      // Check if { is after //
      long int doubleslash = m_Buffer.find_last_of("//",pos);
      if(doubleslash != -1)
        {
        if(this->GetLineNumber(doubleslash) == this->GetLineNumber(pos))
          {
          check = false;
          }
        }

      if(check)
        {
        wantedIndent += size;
        }
      }
    
    if(firstChar) // general case
      {
      // if we are in a comment
      if(this->IsInComments(pos))
        {
        // We check how much space we have in the middle section
        unsigned int nSpaceMiddle = 0;
        while(m_CommentMiddle[nSpaceMiddle] == type)
          {
          nSpaceMiddle++;
          }

        if((*it) == m_CommentMiddle[nSpaceMiddle])
          {
          currentIndent -= nSpaceMiddle;
          }
           
        else
          {
          // We check how much space we have in the end section
          unsigned int nSpaceEnd = 0;
          while(m_CommentEnd[nSpaceEnd] == type)
            {
            nSpaceEnd++;
            }

          if((*it) == m_CommentEnd[nSpaceEnd])
            {
            currentIndent -= nSpaceEnd;
            }
          }
        }

      if((currentIndent != wantedIndent) 
          && !this->IsBetweenChars('<','>',pos,true)
          && !this->IsBetweenChars('(',')',pos,true)
          )
        {
        // If we are inside an enum we do not check indent
        bool isInsideEnum = false;
        long int bracket = m_Buffer.find_last_of('{',pos);
        if(bracket != -1)
          {
          unsigned int l = this->FindPreviousWord(bracket-1,true).size();
          if(this->FindPreviousWord(bracket-l-2,true) == "enum")
            {
            isInsideEnum = true;
            }
          }

        // We check that the previous line is not ending with a semicolon
        // and that the sum of the two lines is more than maxLength
        std::string previousLine = this->GetLine(this->GetLineNumber(pos)-2);
        std::string currentLine = this->GetLine(this->GetLineNumber(pos)-1);
        if(( (previousLine[previousLine.size()-1] != ';')
           && (previousLine.size()+currentLine.size()-currentIndent>maxLength))
          || (isInsideEnum)
          )
          {
          // Do nothing
          }
        else
          {
          Error error;
          error.line = this->GetLineNumber(pos);
          error.line2 = error.line;
          error.number = INDENT;
          error.description = "Indent is wrong ";
          char* val = new char[10];
          sprintf(val,"%d",currentIndent); 
          error.description += val;
          error.description += " (should be ";
          delete [] val;
          val = new char[10];
          sprintf(val,"%d",wantedIndent);
          error.description += val;
          error.description += ")";
          delete [] val;
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }

    if((it != m_Buffer.end()) && ((*it) == '}') && !sindent && !this->IsInComments(pos)) // closing bracket
      {
      bool check = true;
      // Check if { is after //
      long int doubleslash = m_Buffer.find_last_of("//",pos);
      if(doubleslash != -1)
        {
        if(this->GetLineNumber(doubleslash) == this->GetLineNumber(pos))
          {
          check = false;
          }
        }
      if(check)
        {
        wantedIndent -= size;
        }
      }
    
    firstChar = false;
    if(it != m_Buffer.end())
      {
      it++;
      pos++;
      }
    }
 
 return !hasError;
}

/** Init the indentation */
bool Parser::InitIndentation()
{
  m_IdentPositionVector.clear();

  // namespace
  std::vector<long int> namespacevec;

  long int posNamespace = m_BufferNoComment.find("namespace",0);
  while(posNamespace!=-1)
    {
    long int posNamespace1 = m_BufferNoComment.find("{",posNamespace);
    if(posNamespace1 != -1)
      {
      long int posNamespace2 = m_BufferNoComment.find(";",posNamespace);
      if((posNamespace2 == -1) || (posNamespace2 > posNamespace1))
        {
        long int posNamespaceComments = this->GetPositionWithComments(posNamespace1);      
        IndentPosition ind;
        ind.position = posNamespaceComments;
        ind.current = 0;
        ind.after = 0;
        namespacevec.push_back(posNamespaceComments);
        //std::cout << "Found Namespace at: " << this->GetLineNumber(posNamespaceComments) << std::endl;
        m_IdentPositionVector.push_back(ind);
        ind.position = this->FindClosingChar('{','}',posNamespaceComments);
        namespacevec.push_back(ind.position);
        m_IdentPositionVector.push_back(ind);
        }
      }
    posNamespace = m_BufferNoComment.find("namespace",posNamespace+1);
    }

  // Create a list of position specific for namespaces
  std::vector<int> namespacePos;
  std::vector<IndentPosition>::iterator itIdentPos = m_IdentPositionVector.begin();
  while(itIdentPos != m_IdentPositionVector.end())
    {
    namespacePos.push_back((*itIdentPos).position);
    itIdentPos++;
    }

  // Check if the { is the first in the file/function or in a namespace
  long int posClass = m_BufferNoComment.find('{',0);
  while(posClass != -1)
    {
    // We count the number of { and } before posClass
    int nOpen = 0;
    int nClose = 0;  

    long int open = m_BufferNoComment.find('{',0);
    while(open!=-1 && open<posClass)
      {
      bool isNamespace = false;
      // Remove the potential namespaces
      std::vector<int>::const_iterator itN = namespacePos.begin();
      while(itN != namespacePos.end())
        {
        if((*itN)==this->GetPositionWithComments(open))
          {
          isNamespace = true;
          }
        itN++;
        }
      if(!isNamespace)
        {
        nOpen++;
        }
      open = m_BufferNoComment.find('{',open+1);
      }

    long int close = m_BufferNoComment.find('}',0);
    while(close!=-1 && close<posClass)
      {
      bool isNamespace = false;
      // Remove the potential namespaces
      std::vector<int>::const_iterator itN = namespacePos.begin();
      while(itN != namespacePos.end())
        {
        if((*itN)==this->GetPositionWithComments(close))
          {
          isNamespace = true;
          }
        itN++;
        }
      if(!isNamespace)
        {
        nClose++;
        }
      close = m_BufferNoComment.find('}',close+1);
      }

    bool defined = false;
      
    if(nClose == nOpen)
      {
      // Check if this is not the namespace previously defined
      std::vector<long int>::iterator itname = namespacevec.begin();
      while(itname != namespacevec.end())
        {
        if((*itname) == this->GetPositionWithComments(posClass))
          {
          defined = true;
          break;
          }
        itname++;
        }
      }
      
    if((nClose == nOpen) && !defined)
      {
      // translate the position in the buffer position;
      long int posClassComments = this->GetPositionWithComments(posClass); 
      IndentPosition ind;
      ind.position = posClassComments;
      ind.current = 0;
      ind.after = 1;
      m_IdentPositionVector.push_back(ind);
      ind.position = this->FindClosingChar('{','}',posClassComments);      
      ind.current = -1;
      ind.after = -1;
      m_IdentPositionVector.push_back(ind);
      }
    posClass = m_BufferNoComment.find('{',posClass+1);
    }

  // int main()
  long int posMain = m_BufferNoComment.find("main",0);
  while(posMain != -1)
    {
    // Check if the next char is '('
    bool valid = true;
    unsigned long pos = posMain+5;
    while(pos<m_BufferNoComment.size() 
         && (m_BufferNoComment[pos]==' ' 
         || m_BufferNoComment[pos]=='\r'
         || m_BufferNoComment[pos]=='\n')
         )
      {
      pos++;
      }
    
    if(m_BufferNoComment[pos]!='(')
      {
      valid = false;
      }
    if(valid)
      {
      long int bracket = m_BufferNoComment.find('{',posMain+4);
      if(bracket != -1)
        {
        // translate the position in the buffer position;
        long int posMainComments = this->GetPositionWithComments(bracket);      
        IndentPosition ind;
        ind.position = posMainComments;
        ind.current = 0;
        ind.after = 1;
        m_IdentPositionVector.push_back(ind);
        ind.position = this->FindClosingChar('{','}',posMainComments);      
        ind.current = -1;
        ind.after = -1;
        m_IdentPositionVector.push_back(ind);
        }
      }
    posMain = m_BufferNoComment.find("main",posMain+4);
    }

  // switch/case statement
  // for the moment break; restore the indentation
  long int posSwitch = m_BufferNoComment.find("switch",0);
  while(posSwitch != -1)
    {
    // If this is the first case we find the openning { in order to 
    // find the closing } of the switch statement
    long int openningBracket = m_BufferNoComment.find("{",posSwitch);
    long int closingBracket = this->FindClosingChar('{','}',openningBracket,true);        
    long int posColumnComments = this->GetPositionWithComments(closingBracket);      
    IndentPosition ind;
    ind.position = posColumnComments;
    ind.current = -1;
    ind.after = -2; 
    m_IdentPositionVector.push_back(ind);

    // Do the default case
    long int defaultPos = m_BufferNoComment.find("default",openningBracket);
    if(defaultPos != -1)
      {
      long int posColumnComments = this->GetPositionWithComments(defaultPos);      
      IndentPosition ind;
      ind.position = posColumnComments;
      ind.current = -1;
      ind.after = 0; 
      m_IdentPositionVector.push_back(ind);
      }

    long int posCase = m_BufferNoComment.find("case",openningBracket);
    bool firstCase = true;
    long int previousCase = openningBracket;

    while(posCase!= -1 && posCase<closingBracket)
      {
      // Check if we don't have any switch statement inside
      long int insideSwitch = m_BufferNoComment.find("switch",previousCase);
      if(insideSwitch>openningBracket && insideSwitch<posCase)
        {
        // jump to the end of the inside switch/case
        long int insideBracket = m_BufferNoComment.find("{",insideSwitch);
        posCase = this->FindClosingChar('{','}',insideBracket,true);        
        }
      else
        {
        long int posColumnComments = this->GetPositionWithComments(posCase);      
        IndentPosition ind;
        ind.position = posColumnComments;
        if(firstCase)
          {
          ind.current = 0;
          }
        else
          {
          ind.current = -1;
          }
        ind.after = 0; 
          
        m_IdentPositionVector.push_back(ind);
        
        long int column = m_BufferNoComment.find(':',posCase+3);
        if(column != -1)
          {
          // translate the position in the buffer position;
          long int posColumnComments = this->GetPositionWithComments(column);      
          IndentPosition ind;
          ind.position = posColumnComments;
          if(firstCase)
            {
            ind.current = 0;
            ind.after = 1; 
            }
          else
            {
            ind.current = -1;
            ind.after = 0; 
            }
          m_IdentPositionVector.push_back(ind);

          // Sometimes there is a { right after the : we skip it if this is
          // the case
          long int ic = posColumnComments+1;
          while(ic<(long int)m_Buffer.size() 
            && (m_Buffer[ic] == ' ' 
            || m_Buffer[ic] == '\r' 
            || m_Buffer[ic] == '\n'))
            {
            ic++;
            }
          if(m_Buffer[ic] == '{')
            {
            IndentPosition ind;
            ind.position = ic;
            ind.current = 0;
            ind.after = 0; 
            m_IdentPositionVector.push_back(ind);
            ind.position = this->FindClosingChar('{','}',ic);   
            ind.current = 0;
            ind.after = 0; 
            m_IdentPositionVector.push_back(ind);
            }
          }
        }
      firstCase = false;
      previousCase = posCase;
      posCase = m_BufferNoComment.find("case",posCase+1);
      }

    posSwitch = m_BufferNoComment.find("switch",posSwitch+3);
    }

  // Some words should be indented as the previous indent
  this->AddIndent("public:",-1,0);
  this->AddIndent("private:",-1,0);
  this->AddIndent("protected:",-1,0);

  // some words should be always align left
  this->AddIndent("#include",ALIGN_LEFT,0);
  this->AddIndent("#if",ALIGN_LEFT,0);
  this->AddIndent("#ifdef",ALIGN_LEFT,0);
  this->AddIndent("#elif",ALIGN_LEFT,0);
  this->AddIndent("#else",ALIGN_LEFT,0);
  this->AddIndent("#endif",ALIGN_LEFT,0);
  this->AddIndent("#ifndef",ALIGN_LEFT,0);
  this->AddIndent("#define",ALIGN_LEFT,0);
  this->AddIndent("#undef",ALIGN_LEFT,0);

  return true;
}

void Parser::AddIndent(const char* name,long int current,long int after)
{
  long int posPrev = m_Buffer.find(name,0);
  while(posPrev!=-1)
    {
    IndentPosition ind;
    ind.position = posPrev;
    ind.current = current;
    ind.after = after;
    m_IdentPositionVector.push_back(ind);
    posPrev = m_Buffer.find(name,posPrev+1);
    }
}

} // end namespace kws
