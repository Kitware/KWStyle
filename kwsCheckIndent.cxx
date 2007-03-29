/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckIndent.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
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
                         unsigned int maxLength,
                         bool allowCommaIndent)
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

  // Variable to check if we are in a comment or not
  bool isCheckingComment = false;

  // In the case we have #if/#else/#endif we want to ignore the #else section
  // if we have some '{' not closed
  // #if 
  //   {
  // #else
  //   {
  // #endif
  this->ComputeIfElseEndifList();

  // Create the indentation
  this->InitIndentation();

  // If we do not want to check the header
  if(doNotCheckHeader)
    {
    unsigned long fileSize = 0;
    // if the file is specified
    if(m_HeaderFilename.size() > 0)
      {
      std::ifstream file;
      file.open(m_HeaderFilename.c_str(), std::ios::binary | std::ios::in);
      if(!file.is_open())
        {
        std::cout << "Cannot open file: " << m_HeaderFilename << std::endl;
        return false;
        }

      file.seekg(0,std::ios::end);
      fileSize = file.tellg();
      file.close();
      }
    else 
      {
      // we look at the first '*/' in the file which indicated the end of the current header
      // This assume that there is an header at some point  
      long int endHeader = m_Buffer.find("*/",0);
      if(endHeader>0)
        {
        fileSize = endHeader;
        }
      }

    // We skip the header
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
    // If we should skip the line
    bool skip = this->IsInElseForbiddenSection(pos);

    if((*it) == type || (*it)=='\r' || skip)
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
        bool returnError = true;
        // We check that the previous line is not ending with a semicolon
        // and that the sum of the two lines is more than maxLength
        std::string previousLine = this->GetLine(this->GetLineNumber(pos)-2);
        std::string currentLine = this->GetLine(this->GetLineNumber(pos)-1);
        if( (previousLine[previousLine.size()-1] != ';')
           && (previousLine.size()+currentLine.size()-currentIndent>maxLength)
          )
          {
          returnError = false;
          }

        // We check that any definitions of public:, private: and protected:
        // are not within a double class (class inside a class)
        // WARNING: We just ignore the error at that point (maybe more checking
        // will be necessary)
        if(sindent->name == "public:" 
          || sindent->name == "protected:" 
          || sindent->name == "private:"
          )
          {
          int inClass = this->IsInClass(this->GetPositionWithoutComments(pos));
          if(inClass>1)
            {
            returnError = false;
            }
          }

        // Check if we are inside a macro. If yes we disable the checking of the ident
        // (This is too complex for macros)
        if(returnError)
          {
          // We are in a macro if we have
          // '#define foo' and the line finishs with '\'
          long int begMacro = m_Buffer.find("#define",0);
          while(begMacro!=-1)
            {
            // Find the end of the macro
            long int endMacro = m_Buffer.find("\r",begMacro);
            while(endMacro>0 && m_Buffer[endMacro-1]=='\\')
              {
              endMacro = m_Buffer.find("\r",endMacro+1);
              }

            if(endMacro!=-1 && (long int)pos<endMacro && (long int)pos>begMacro)
              {
              returnError = false;
              break;
              }
            begMacro = m_Buffer.find("#define",endMacro);
            }
          }

        if(returnError)
          {
          Error error;
          error.line = this->GetLineNumber(pos);
          error.line2 = error.line;
          error.number = INDENT;
          error.description = "Special Indent is wrong ";
          char* val = new char[10];
          sprintf(val,"%d",currentIndent); 
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
    else if((it != m_Buffer.end()) && ((*it) == '{') 
             //&& !this->IsInComments(pos)
             && !isCheckingComment
             && !this->IsBetweenQuote(pos,true)
             ) // openning bracket
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
      // If we are in a comment
      if(this->IsInComments(pos))
        {
        if(this->IsInAnyComments(pos))
          {
          isCheckingComment = true;
          }
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
            isCheckingComment = false;
            }
          }
        }
      else
        {
        isCheckingComment = false;
        }

      if(isCheckingComment && !this->IsInAnyComments(pos))
        {
        std::cout << "WARNING: There is a problem with the comments on line " << this->GetLineNumber(pos) << std::endl;
        // We check how much space we have in the end section
        unsigned int nSpaceEnd = 0;
        while(m_CommentEnd[nSpaceEnd] == type)
          {
          nSpaceEnd++;
          }       
        currentIndent -= nSpaceEnd;
        isCheckingComment = false;
        }

      if((currentIndent != wantedIndent) 
          && !this->IsBetweenCharsFast('<','>',pos,true)
          && !this->IsBetweenCharsFast('(',')',pos,true)
          && !this->IsBetweenChars('(',')',pos,true) 
          && !this->IsBetweenChars('<','>',pos,true) 
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

        bool reportError = true;

        // We check that the previous line is not ending with a semicolon
        // and that the sum of the two lines is more than maxLength
        std::string previousLine = this->GetLine(this->GetLineNumber(pos)-2);
        std::string currentLine = this->GetLine(this->GetLineNumber(pos)-1);
        if(( (previousLine[previousLine.size()-1] != ';')
           && (previousLine.size()+currentLine.size()-currentIndent>maxLength))
          || (isInsideEnum)
          )
          {
          reportError = false;
          }
        // Check if the line start with '<<' if this is the case we ignore it
        else if((*it) == '<' && (*(it+1) == '<'))
          {
          long int posInf = previousLine.find("<<");
          if((posInf != -1) && (posInf == currentIndent+1))
            {
            reportError = false;
            } 
          }
        // We don't care about everything between the class and '{'
        else
          {
          long int classPos = m_Buffer.find("class",0);
          while(classPos!=-1)
            {
            long int endClass = m_Buffer.find("{",classPos);
            if(endClass!=-1 && (long int)pos<endClass && (long int)pos>classPos)
              {
              reportError = false;
              break;
              }
            classPos = m_Buffer.find("class",classPos+1);
            }
          }

        // If the ident is between a '=' and a ';' we ignore
        // This is for lists. THIS IS NOT A STRICT CHECK. Might be missing some.
        if(reportError)
          {
          long int classPos = m_BufferNoComment.find("=",0);
          while(classPos!=-1)
            {
            long int posNoCommments = this->GetPositionWithoutComments(pos);
            long int endClass = m_BufferNoComment.find(";",classPos);
            if(endClass!=-1 && (long int)posNoCommments<endClass && (long int)posNoCommments>classPos)
              {
              reportError = false;
              break;
              }
            classPos = m_Buffer.find("=",classPos+1);
            }
          }

        // Check if we are inside a macro. If yes we disable the checking of the ident
        // (This is too complex for macros)
        if(reportError)
          {
          // We are in a macro if we have
          // '#define foo' and the line finishs with '\'
          long int begMacro = m_Buffer.find("#define",0);
          while(begMacro!=-1)
            {
            // Find the end of the macro
            long int endMacro = m_Buffer.find("\r",begMacro);
            while(endMacro>0 && m_Buffer[endMacro-1]=='\\')
              {
              endMacro = m_Buffer.find("\r",endMacro+1);
              }

            if(endMacro!=-1 && (long int)pos<endMacro && (long int)pos>begMacro)
              {
              reportError = false;
              break;
              }
            begMacro = m_Buffer.find("#define",endMacro);
            }
          }

        // If we allowCommaIndentation:
        // if(myvalue,
        //    myvalue2)
        if(allowCommaIndent && reportError)
          {
          // Check the end of the previous line if we have a comma
          long int j = previousLine.size()-1;
          while(j>0)
            {
            if(previousLine[j] != ' '
               && previousLine[j] != '\n'
               && previousLine[j] != '\r')
              {
              break;
              }
            j--;
            }

          if(previousLine[j] == ',')
            {
            reportError = false;
            }
          }

        if(reportError)
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

    if((it != m_Buffer.end()) && ((*it) == '}') 
       && !sindent 
       //&& !this->IsInComments(pos)
       && !isCheckingComment
       && !this->IsBetweenQuote(pos,true)) // closing bracket
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

  while(posClass!= -1 && this->IsInElseForbiddenSection(this->GetPositionWithComments(posClass)))
    {
    posClass = m_BufferNoComment.find('{',posClass+1);
    }

  while(posClass != -1)
    {
    // We count the number of { and } before posClass
    int nOpen = 0;
    int nClose = 0;  

    long int open = m_BufferNoComment.find('{',0);
    while(open!=-1 && open<posClass)
      {
      if(!this->IsInElseForbiddenSection(this->GetPositionWithComments(open)))
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
        }
      open = m_BufferNoComment.find('{',open+1);
      }

    long int close = m_BufferNoComment.find('}',0);
    while(close!=-1 && close<posClass)
      {
      if(!this->IsInElseForbiddenSection(this->GetPositionWithComments(close)))
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
      
      // The current indent should be -1 unless we are right after the openning
      // bracket. In that case the current indent should be 0;
      long int j=defaultPos-1;
      while(j>0)
        {
        if(m_BufferNoComment[j] != ' '
          && m_BufferNoComment[j] != '\n'
          && m_BufferNoComment[j] != '\r'
          )
          {
          break;
          }
        j--;
        }
        
      if(j == openningBracket)
        {
        ind.current = 0;
        }
      else
        {
        ind.current = -1;
        }

      ind.after = 0;
      m_IdentPositionVector.push_back(ind);
      // Find the ':' after the default
      long int column = m_BufferNoComment.find(":",defaultPos+1);
      column = this->GetPositionWithComments(column);      
      
      // Sometimes there is a { right after the : we skip it if this is
      // the case
      long int ic = column+1;
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
        // Make sure that we are not checing '::'
        while(column+1<(long int)m_BufferNoComment.size()
          && m_BufferNoComment[column+1]==':')
          {
          column = m_BufferNoComment.find(':',column+2);
          }

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
  //this->AddIndent("#ifdef",ALIGN_LEFT,0); // #if is taking care of it
  this->AddIndent("#elif",ALIGN_LEFT,0);
  this->AddIndent("#else",ALIGN_LEFT,0);
  this->AddIndent("#endif",ALIGN_LEFT,0);
  //this->AddIndent("#ifndef",ALIGN_LEFT,0); // #if is taking care of it
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
    ind.name = name;
    m_IdentPositionVector.push_back(ind);
    posPrev = m_Buffer.find(name,posPrev+1);
    }
}

/** Return true if the current position with comments is in the
 *  #else section of the forbiden part.
 *  In case we have something like:
 *  #if
 *  {
 *  #else
 *  {
 *  #endif */
bool Parser::IsInElseForbiddenSection(long int pos)
{
  IfElseEndifListType::const_iterator itLS = m_IfElseEndifList.begin();
  while(itLS != m_IfElseEndifList.end())
    {
    if((long int)pos>(*itLS).first && (long int)pos<(*itLS).second)
      {
      return true;
      }
    itLS++;
    }
  return false;
}


} // end namespace kws
