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
                         bool allowBlockLine)
{
  m_TestsDone[INDENT] = true;
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
  if(itype == TABS) {type = '\t';}

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
      //std::cout << "Got l=" << this->GetLineNumber(sindent->position) << " : " << wantedIndent << " : " << sindent->current << " : " << sindent->after << std::endl;
      if(sindent->current == ALIGN_LEFT)
        {
        wanted = 0;
        }

      else if(currentIndent != wanted)
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
      wantedIndent += size*sindent->after;
      firstChar = false;
      }
    else if((it != m_Buffer.end()) && ((*it) == '{')) // openning bracket
      {
      wantedIndent += size;
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

    if((it != m_Buffer.end()) && ((*it) == '}') && !sindent) // closing bracket
      {
      wantedIndent -= size;
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

  // Check if the { is the first in the file/function or in a namespace
  long int posClass = m_BufferNoComment.find('{',0);
  while(posClass != -1)
    {
    bool defined = false;
    // Check if this is not the namespace previously defined
    std::vector<IndentPosition>::iterator itIdentPos = m_IdentPositionVector.begin();
    while(itIdentPos != m_IdentPositionVector.end())
      {
      if((*itIdentPos).position == this->GetPositionWithComments(posClass))
        {
        defined = true;
        break;
        }
      itIdentPos++;
      }

    if(!defined)
      {
      // Look backward
      long int close = 1;
      long int i = posClass-1;
      bool found = false;

      while((close!=2) && (i>0))
        {
        if(m_BufferNoComment[i] == '{')
          {
          close++;
          }
        if(m_BufferNoComment[i] == '}')
          {
          close--;       
          }
       if(close == 2)
         {
         found = true;
         break;
         }
        i--;
        }

      if(found)
        {
        bool defined = false;
        // Check if this is not the namespace previously defined
        std::vector<long int>::iterator itname = namespacevec.begin();
        while(itname != namespacevec.end())
          {
          if((*itname) == this->GetPositionWithComments(i))
            {
            defined = true;
            break;
            }
          itname++;
          }

        if(!defined)
          {
          found = false;
          }
        }

      if(found)
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
      }
    posClass = m_BufferNoComment.find('{',posClass+1);
    }

  
  /*// class
  long int posClass = this->GetClassPosition(0);
  while(posClass != -1)
    {
    long int bracket = m_BufferNoComment.find('{',posClass-2);
    if(bracket != -1)
      {
      // translate the position in the buffer position;
      long int posClassComments = this->GetPositionWithComments(bracket);      
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
    posClass = this->GetClassPosition(posClass+1);
    }
  */

  // Some words should be indented as the previous indent
  this->AddIndent("public:",-1,0);
  this->AddIndent("private:",-1,0);
  this->AddIndent("protected:",-1,0);

  // some words should be always align left
  this->AddIndent("#include",ALIGN_LEFT,0);
  this->AddIndent("#if",ALIGN_LEFT,0);
  this->AddIndent("#elif",ALIGN_LEFT,0);
  this->AddIndent("#endif",ALIGN_LEFT,0);
  this->AddIndent("#ifndef",ALIGN_LEFT,0);
  this->AddIndent("#define",ALIGN_LEFT,0);

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
