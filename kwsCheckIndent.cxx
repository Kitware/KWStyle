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

#include <string.h>

namespace kws {

#define ALIGN_LEFT -99999

/** Extract the current line from pos to LF */
std::string Parser::ExtractLine(size_t pos)
{
  size_t p = m_Buffer.find("\n",pos);
  if(p>pos)
    {
    return m_Buffer.substr(pos,p-pos-1);
    }
  return "";
}

/** Return the current ident */
int Parser::GetCurrentIdent(std::string line,char type)
{
  int indent = 0;
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
  constexpr size_t length = 11;
  char* val = new char[length];
  snprintf(val,length,"%ld ",size);
  m_TestsDescription[INDENT] += val;
  if(itype == TAB)
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

  // Print the initial indentation
  /*std::vector<IndentPosition>::const_iterator itIndent
   * = m_IdentPositionVector.begin();
  while(itIndent != m_IdentPositionVector.end())
    {
    std::cout << this->GetLineNumber((*itIndent).position) << std::endl;
    std::cout << (*itIndent).current << std::endl;
    std::cout << (*itIndent).after << std::endl;
    std::cout << (*itIndent).name.c_str() << std::endl;
    itIndent++;
    }*/

  // If we do not want to check the header
  if(doNotCheckHeader)
    {
    unsigned long fileSize = 0;
    // if the file is specified
    if (!m_HeaderFilename.empty()) {
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
    } else {
      // we look at the first '*/' in the file which indicated the end
      // of the current header
      // This assume that there is an header at some point
      auto endHeader = static_cast<long int>(m_Buffer.find("*/", 0));
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
  if(itype == kws::TAB) {type = '\t';}

  int wantedIndent = 0;

  // We extract the first line and compute the number of spaces/tabs at
  // the beginning
  std::string line = this->ExtractLine(pos);
  int currentIndent = this->GetCurrentIdent(line,type);
  bool firstChar = true;

  // We start to check
  while(it != m_Buffer.end())
    {
    // If we should skip the line
    bool skip = this->IsInElseForbiddenSection(pos);

    if( strncmp(&(*it), "typedef", 7 ) == 0 ) 
      {
      while( it != m_Buffer.end() && (*it) != ';' )
        {
        ++it;
        ++pos;
        }
      continue;
      }

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
      std::string localline = this->ExtractLine(pos);
      currentIndent = this->GetCurrentIdent(localline,type);
      firstChar = true;
      continue;
      }

    // Check if pos is in the list of positions
      auto itIdentPos = m_IdentPositionVector.begin();
      IndentPosition *sindent = nullptr;
      while (itIdentPos != m_IdentPositionVector.end()) {
        if ((*itIdentPos).position == pos) {
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
           && (previousLine.size()+currentLine.size()-currentIndent>0.9*maxLength)
          )
          {
          returnError = false;
          }

        // Check for special characters
        if(previousLine[previousLine.size()-1] != ';')
          {
          // Check if we have a << at the beginning of the current line
          auto posSpecialChar = static_cast<long int>(currentLine.find("<<"));
          if(posSpecialChar != -1)
            {
            returnError = false;
            for(long int i=0;i<posSpecialChar;i++)
              {
              if(currentLine[i] != '\r' && currentLine[i] != '\n' && currentLine[i] != ' ' && currentLine[i] != '\t')
                {
                returnError = true;
                break;
                }
              }
            }
          }

        // We check that any definitions of public:, private: and protected:
        // are not within a double class (class inside a class)
        // WARNING: We just ignore the error at that point (maybe more checking
        // will be necessary)
        if(sindent->name == "public:"
          || sindent->name == "protected:"
          || sindent->name == "private:"
          || sindent->name == "signals:"
          || sindent->name == "public slots:"
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
          auto begMacro = static_cast<long int>(m_Buffer.find("#define", 0));
          while(begMacro!=-1)
            {
            // Find the end of the macro
            auto endMacro =
                static_cast<long int>(m_Buffer.find("\r", begMacro));
            while(endMacro>0 && m_Buffer[endMacro-1]=='\\')
              {
              endMacro = static_cast<long int>(m_Buffer.find("\r",endMacro+1));
              }

            if(endMacro!=-1 && (long int)pos<endMacro && (long int)pos>begMacro)
              {
              returnError = false;
              break;
              }
            begMacro = static_cast<long int>(m_Buffer.find("#define",endMacro));
            }
          }

        if(returnError)
          {
          Error error;
          error.line = this->GetLineNumber(pos);
          error.line2 = error.line;
          error.number = INDENT;
          error.description = "Special Indent is wrong ";
          constexpr size_t length = 11;
          char* localval = new char[length];
          snprintf(localval,length,"%d",currentIndent);
          error.description += localval;
          error.description += " (should be ";
          delete [] localval;
          localval = new char[length];
          snprintf(localval,length,"%ld",wanted);
          error.description += localval;
          error.description += ")";
          delete [] localval;
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
             //&& !this->IsBetweenQuote(pos,true)
             && !(
            (!this->IsInAnyComments(pos) && this->IsBetweenQuote(this->GetPositionWithoutComments(pos),false))
            || (this->IsInAnyComments(pos) && this->IsBetweenQuote(pos,true)))
             ) // openning bracket
      {
      bool check = true;
      // Check if { is after // [THIS CHECK IS NOT USEFULL ANYMORE]
      auto doubleslash = static_cast<long int>(m_Buffer.rfind("//", pos));
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
          if(currentIndent>0)
            {
            currentIndent -= nSpaceMiddle;
            }
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

      bool inComment = this->IsInAnyComments(pos);

      if(isCheckingComment && !inComment)
        {
        Warning warning;
        warning.line = this->GetLineNumber(pos);
        warning.line2 = this->GetLineNumber(pos);
        warning.description = "There is a problem with the comments";
        warning.number = INDENT;
        m_WarningList.push_back(warning);

        // We check how much space we have in the end section
        unsigned int nSpaceEnd = 0;
        while(m_CommentEnd[nSpaceEnd] == type)
          {
          nSpaceEnd++;
          }
        currentIndent -= nSpaceEnd;
        isCheckingComment = false;
        }

      unsigned int poswithoutcomment = static_cast<long int>(this->GetPositionWithoutComments(pos));

      if((currentIndent != wantedIndent)
        && ((inComment
          && !this->IsBetweenCharsFast('<','>',pos,true)
          && !this->IsBetweenCharsFast('(',')',pos,true)
          && !this->IsBetweenChars('(',')',pos,true)
          && !this->IsBetweenChars('<','>',pos,true))
          ||
          (
          !inComment
          && !this->IsBetweenCharsFast('<','>',poswithoutcomment,false)
          && !this->IsBetweenCharsFast('(',')',poswithoutcomment,false)
          && !this->IsBetweenChars('(',')',poswithoutcomment,false)
          && !this->IsBetweenChars('<','>',poswithoutcomment,false)))
          )
        {
        // If we are inside an enum we do not check indent
        bool isInsideEnum = false;
        auto bracket = static_cast<long int>(m_Buffer.find_last_of('{', pos));
        if(bracket != -1)
          {
          unsigned int l = static_cast<long int>(this->FindPreviousWord(bracket-1,true).size());
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
           && (previousLine.size()+currentLine.size()-currentIndent>0.9*maxLength))
          || (isInsideEnum)
          )
          {
          reportError = false;
          }
        // Check if the line start with '<<' if this is the case we ignore it
        else if( strncmp( &(*it), "<<", 2) == 0  || 
          ( previousLine.size() > 6
          && previousLine.compare( previousLine.size()-2, 2, "<<" ) == 0) )
          {
          reportError = false;
          }
        // Catch some instances of long variable declarations using
        // typedefed types that end in "Type".
        else if( previousLine.size() > 6
          && previousLine.compare( previousLine.size()-4, 4, "Type" ) == 0 )
          {
          reportError = false;
          }
        // We don't care about everything between the class and '{'
        else
          {
            auto classPos = static_cast<long int>(m_Buffer.find("class", 0));
            while (classPos != -1) {
              auto endClass =
                  static_cast<long int>(m_Buffer.find("{", classPos));
              if (endClass != -1 && (long int)pos < endClass &&
                  (long int)pos > classPos) {
                reportError = false;
                break;
              }
            classPos = static_cast<long int>(m_Buffer.find("class",classPos+1));
            }
          }

        // If the ident is between a '=' and a ';' we ignore
        // This is for lists. THIS IS NOT A STRICT CHECK. Might be missing some.
        if(reportError)
          {
          auto classPos = static_cast<long int>(m_BufferNoComment.find("=", 0));
          while(classPos!=-1)
            {
            auto posNoCommments =
                static_cast<long int>(this->GetPositionWithoutComments(pos));
            auto endClass =
                static_cast<long int>(m_BufferNoComment.find(";", classPos));
            if(endClass!=-1 && (long int)posNoCommments<endClass && (long int)posNoCommments>classPos)
              {
              reportError = false;
              break;
              }
            classPos = static_cast<long int>(m_BufferNoComment.find("=",classPos+1));
            }
          }

        // If the ident is between a ':' and a '{' we ignore
        // This is for the constructor.
        if(reportError)
          {
          auto classPos = static_cast<long int>(m_BufferNoComment.find(":", 0));
          while(classPos!=-1)
            {
            auto posNoCommments =
                static_cast<long int>(this->GetPositionWithoutComments(pos));
            auto endConstructor =
                static_cast<long int>(m_BufferNoComment.find("{", classPos));

            if(endConstructor!=-1 && (long int)posNoCommments<endConstructor && (long int)posNoCommments>classPos)
              {
              reportError = false;
              break;
              }
            classPos = static_cast<long int>(m_BufferNoComment.find(":",classPos+1));
            }
          }

        // If the ident is between 'return' and ';' we ignore
        // Ideally we should have a strict check
        if(reportError)
          {
          auto classPos =
              static_cast<long int>(m_BufferNoComment.find("return", 0));
          while(classPos!=-1)
            {
            auto posNoCommments =
                static_cast<long int>(this->GetPositionWithoutComments(pos));
            auto endConstructor =
                static_cast<long int>(m_BufferNoComment.find(";", classPos));

            if(endConstructor!=-1 && (long int)posNoCommments<endConstructor && (long int)posNoCommments>classPos)
              {
              reportError = false;
              break;
              }
            classPos = static_cast<long int>(m_BufferNoComment.find("return",classPos+1));
            }
          }

        // Check if we are inside a macro. If yes we disable the checking of the ident
        // (This is too complex for macros)
        if(reportError)
          {
          // We are in a macro if we have
          // '#define foo' and the line finishs with '\'
          auto begMacro = static_cast<long int>(m_Buffer.find("#define", 0));
          while(begMacro!=-1)
            {
            // Find the end of the macro
            auto endMacro =
                static_cast<long int>(m_Buffer.find("\r", begMacro));
            while(endMacro>0 && m_Buffer[endMacro-1]=='\\')
              {
              endMacro = static_cast<long int>(m_Buffer.find("\r",endMacro+1));
              }

            if(endMacro!=-1 && (long int)pos<endMacro && (long int)pos>begMacro)
              {
              reportError = false;
              break;
              }
            begMacro = static_cast<long int>(m_Buffer.find("#define",endMacro));
            }
          }

        // If we allowCommaIndentation:
        // if(myvalue,
        //    myvalue2)
        if(allowCommaIndent && reportError)
          {
          // Check the end of the previous line if we have a comma
          auto j = static_cast<long int>(previousLine.size() - 1);
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

        if( currentIndent < 0 )
          {
          reportError = false;
          }

        if(reportError)
          {
          Error error;
          error.line = this->GetLineNumber(pos);
          error.line2 = error.line;
          error.number = INDENT;
          error.description = "Indent is wrong ";
          constexpr size_t length = 11;
          char* localval = new char[length];
          snprintf(localval,length,"%d",currentIndent);
          error.description += localval;
          error.description += " (should be ";
          delete [] localval;
          localval = new char[length];
          snprintf(localval,length,"%d",wantedIndent);
          error.description += localval;
          error.description += ")";
          delete [] localval;
          m_ErrorList.push_back(error);
          hasError = true;
          }
        }
      }

    if((it != m_Buffer.end()) && ((*it) == '}')
       && !sindent
       //&& !this->IsInComments(pos)
       && !isCheckingComment
       //&& !this->IsBetweenQuote(pos,true)
       && !(
       (!this->IsInAnyComments(pos) && this->IsBetweenQuote(this->GetPositionWithoutComments(pos),false))
       || (this->IsInAnyComments(pos) && this->IsBetweenQuote(pos,true)))
      ) // closing bracket
      {
      bool check = true;
      // Check if { is after //
      auto doubleslash = static_cast<long int>(m_Buffer.rfind("//", pos));
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


/** Check if the current position is a valid switch statement */
bool Parser::CheckValidSwitchStatement(unsigned int posSwitch)
{
  if((m_BufferNoComment[posSwitch-1]!='\n'
     && m_BufferNoComment[posSwitch-1]!=' '
     && posSwitch-1 != 0) ||
     (m_BufferNoComment.size() > posSwitch+6
      && m_BufferNoComment[posSwitch+6] != ' '
      && m_BufferNoComment[posSwitch+6] != '('))
   {
   return false;
   }
  return true;
}


/** Init the indentation */
bool Parser::InitIndentation()
{
  m_IdentPositionVector.clear();

  // namespace
  std::vector<size_t> namespacevec;

  size_t posNamespace = m_BufferNoComment.find("namespace",0);
  while(posNamespace!=std::string::npos)
    {
    size_t posNamespace1 = m_BufferNoComment.find("{",posNamespace);
    if(posNamespace1 != std::string::npos)
      {
      size_t posNamespace2 = m_BufferNoComment.find(";",posNamespace);
      if((posNamespace2 == std::string::npos) || (posNamespace2 > posNamespace1))
        {
        size_t posNamespaceComments = this->GetPositionWithComments(posNamespace1);
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
  std::vector<size_t> namespacePos;
  auto itIdentPos = m_IdentPositionVector.begin();
  while(itIdentPos != m_IdentPositionVector.end())
    {
    namespacePos.push_back((*itIdentPos).position);
    itIdentPos++;
    }

  // Check if the { is the first in the file/function or in a namespace
  size_t posClass = m_BufferNoComment.find('{',0);

  while(posClass!= std::string::npos && this->IsInElseForbiddenSection(this->GetPositionWithComments(posClass)))
    {
    posClass = m_BufferNoComment.find('{',posClass+1);
    }

  while(posClass != std::string::npos)
    {
    // We count the number of { and } before posClass
    unsigned int nOpen = 0;
    unsigned int nClose = 0;

    size_t open = m_BufferNoComment.find('{',0);
    while(open!=std::string::npos && open<posClass)
      {
      if(!this->IsInElseForbiddenSection(this->GetPositionWithComments(open))
        && !this->IsBetweenQuote(open)
        )
        {
        bool isNamespace = false;
        // Remove the potential namespaces
        std::vector<size_t>::const_iterator itN = namespacePos.begin();
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

    size_t close = m_BufferNoComment.find('}',0);
    while(close!=std::string::npos && close<posClass)
      {
      if(!this->IsInElseForbiddenSection(this->GetPositionWithComments(close))
        && !this->IsBetweenQuote(close)
        )
        {
        bool isNamespace = false;
        // Remove the potential namespaces
        std::vector<size_t>::const_iterator itN = namespacePos.begin();
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
      auto itname = namespacevec.begin();
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
      size_t posClassComments = this->GetPositionWithComments(posClass);
      IndentPosition ind;
      ind.position = posClassComments;
      ind.current = 0;
      ind.after = 1;
      m_IdentPositionVector.push_back(ind);
      ind.position = this->FindClosingChar('{','}',posClassComments);
      while(this->IsBetweenQuote(ind.position,true))
        {
        ind.position = this->FindClosingChar('{','}',ind.position+1);
        }
      ind.current = -1;
      ind.after = -1;
      m_IdentPositionVector.push_back(ind);
      }
    posClass = m_BufferNoComment.find('{',posClass+1);
    }

  // int main()
  size_t posMain = m_BufferNoComment.find("main",0);
  while(posMain != std::string::npos)
    {
    // Check if the next char is '('
    bool valid = true;
    auto pos = static_cast<unsigned long>(posMain + 5);
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
      size_t bracket = m_BufferNoComment.find('{',posMain+4);
      if(bracket != std::string::npos)
        {
        // translate the position in the buffer position;
        size_t posMainComments = this->GetPositionWithComments(bracket);
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
  size_t posSwitch = m_BufferNoComment.find("switch",0);
  while(posSwitch != std::string::npos)
    {
    // Check that it is a valid switch statement
    if(!this->CheckValidSwitchStatement(static_cast<unsigned int>(posSwitch)))
     {
     posSwitch = m_BufferNoComment.find("switch",posSwitch+1);
     continue;
     }

    // If this is the first case we find the openning { in order to
    // find the closing } of the switch statement
    size_t openningBracket = m_BufferNoComment.find("{",posSwitch);
    size_t closingBracket = this->FindClosingChar('{','}',openningBracket,true);
    size_t posColumnComments = this->GetPositionWithComments(closingBracket);
    IndentPosition ind;
    ind.position = posColumnComments;
    ind.current = -1;
    ind.after = -2;
    m_IdentPositionVector.push_back(ind);

    // Do the default case
    size_t defaultPos = m_BufferNoComment.find("default",openningBracket);
    if(defaultPos > closingBracket)
      {
      defaultPos = std::string::npos;
      }

    // We need to make sure that there is no "switch" statement nested
    size_t nestedSwitch = m_BufferNoComment.find("switch",posSwitch+1);
    while(nestedSwitch != std::string::npos)
      {
        if(!this->CheckValidSwitchStatement(static_cast<unsigned int>(nestedSwitch)))
        {
        nestedSwitch = m_BufferNoComment.find("switch",nestedSwitch+1);
        continue;
        }

      if(nestedSwitch < defaultPos)
        {
        defaultPos = m_BufferNoComment.find("default",defaultPos+1);
        }
      else
        {
        break;
        }
      nestedSwitch = m_BufferNoComment.find("switch",nestedSwitch+1);
      }

    if(defaultPos != std::string::npos)
      {
      size_t localposColumnComments = this->GetPositionWithComments(defaultPos);
      IndentPosition localind;
      localind.position = localposColumnComments;

      // The current indent should be -1 unless we are right after the openning
      // bracket. In that case the current indent should be 0;
      size_t j=defaultPos-1;
      while(j!=std::string::npos)
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
        localind.current = 0;
        }
      else
        {
        localind.current = -1;
        }

      localind.after = 0;
      m_IdentPositionVector.push_back(localind);
      // Find the ':' after the default
      size_t column = m_BufferNoComment.find(":",defaultPos+1);
      column = this->GetPositionWithComments(column);

      // Sometimes there is a { right after the : we skip it if this is
      // the case
      size_t ic = column+1;
      while(ic<m_Buffer.size()
            && (m_Buffer[ic] == ' '
            || m_Buffer[ic] == '\r'
            || m_Buffer[ic] == '\n'))
        {
        ic++;
        }
      if(m_Buffer[ic] == '{')
        {
        IndentPosition tempind;
        tempind.position = ic;
        tempind.current = 0;
        tempind.after = 0;
        m_IdentPositionVector.push_back(tempind);
        tempind.position = this->FindClosingChar('{','}',ic);
        tempind.current = 0;
        tempind.after = 0;
        m_IdentPositionVector.push_back(tempind);
        }
      }

    size_t posCase = m_BufferNoComment.find("case",openningBracket);
    bool firstCase = true;
    size_t previousCase = openningBracket;

    while(posCase!= std::string::npos && posCase<closingBracket)
      {
      // Check if we don't have any switch statement inside
      size_t insideSwitch = m_BufferNoComment.find("switch",previousCase);
      if(insideSwitch>openningBracket && insideSwitch<posCase)
        {
        // jump to the end of the inside switch/case
        size_t insideBracket = m_BufferNoComment.find("{",insideSwitch);
        posCase = this->FindClosingChar('{','}',insideBracket,true);
        }
      else
        {
        size_t localposColumnComments = this->GetPositionWithComments(posCase);
        IndentPosition localindtemp;
        localindtemp.position = localposColumnComments;
        if(firstCase)
          {
          localindtemp.current = 0;
          }
        else
          {
          localindtemp.current = -1;
          }
        localindtemp.after = 0;

        m_IdentPositionVector.push_back(localindtemp);

        size_t column = m_BufferNoComment.find(':',posCase+3);
        // Make sure that we are not checing '::'
        while(column+1<m_BufferNoComment.size()
          && m_BufferNoComment[column+1]==':')
          {
          column = m_BufferNoComment.find(':',column+2);
          }

        if(column != std::string::npos)
          {
          // translate the position in the buffer position;
          localposColumnComments = this->GetPositionWithComments(column);
          IndentPosition localind;
          localind.position = localposColumnComments;
          if(firstCase)
            {
            localind.current = 0;
            localind.after = 1;
            }
          else
            {
            localind.current = -1;
            localind.after = 0;
            }
          m_IdentPositionVector.push_back(localind);

          // Sometimes there is a { right after the : we skip it if this is
          // the case
          size_t ic = localposColumnComments+1;
          while(ic<m_Buffer.size()
            && (m_Buffer[ic] == ' '
            || m_Buffer[ic] == '\r'
            || m_Buffer[ic] == '\n'))
            {
            ic++;
            }
          if(m_Buffer[ic] == '{')
            {
            IndentPosition tempind;
            tempind.position = ic;
            tempind.current = 0;
            tempind.after = 0;
            m_IdentPositionVector.push_back(tempind);
            tempind.position = this->FindClosingChar('{','}',ic);
            tempind.current = 0;
            tempind.after = 0;
            m_IdentPositionVector.push_back(tempind);
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
  this->AddIndent("signals:",-1,0);
  this->AddIndent("public slots:",-1,0);
  this->AddIndent("private slots:",-1,0);
  this->AddIndent("protected slots:",-1,0);

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

void Parser::AddIndent(const char* name,int current,int after)
{
  size_t posPrev = m_Buffer.find(name,0);
  while(posPrev!=std::string::npos)
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
bool Parser::IsInElseForbiddenSection(size_t pos)
{
  IfElseEndifListType::const_iterator itLS = m_IfElseEndifList.begin();
  while(itLS != m_IfElseEndifList.end())
    {
    if(pos>(*itLS).first && pos<(*itLS).second)
      {
      return true;
      }
    itLS++;
    }
  return false;
}


} // end namespace kws
