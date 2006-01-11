/*=========================================================================

  Program:   ITKXML
  Module:    kwsCheckIndent.cxx
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

namespace kws {

/** Check the indent size */
bool Parser::CheckIndent(IndentType itype,unsigned long size,bool doNotCheckHeader)
{

  m_TestsDone[INDENT] = true;

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

} // end namespace kws
