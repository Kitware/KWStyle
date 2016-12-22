/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckVariablePerLine.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the number of statements per line */
bool Parser::CheckVariablePerLine(unsigned long max)
{
  m_TestsDone[VARIABLEPERLINE] = true;
  char* val = new char[255];
  sprintf(val,"Variables per line = %ld max",max);
  m_TestsDescription[VARIABLEPERLINE] = val;
  delete [] val;

  // For the moment only standard types are defined.
  // We might be able to do more with finding typedefs
  #define ntypes 12
  const char* types[ntypes] = {"int","unsigned int",
   "char","unsigned char",
   "short","unsigned short",
   "long","unsigned long",
   "float","double","void","long int"};

  bool hasError = false;
  for(unsigned int i = 0;i<ntypes;i++)
    {
    std::string typeToFind = types[i];
    typeToFind += " ";
    size_t posType = m_BufferNoComment.find(typeToFind,0);
    while(posType != std::string::npos)
      {
      // Check that this is the first word
      bool firstWord = false;
      long int pos=static_cast<long int>(posType);
      pos--;
      while((pos>0) && (m_BufferNoComment[pos]==' '))
        {
        pos--;
        }
      if((pos<=0) || (m_BufferNoComment[pos]=='\n'))
        {
        firstWord = true;
        }

      if(firstWord)
        {
        std::string line = this->GetLine(this->GetLineNumber(posType,true)-1);

        // Check if we have any comments
        int poscom = static_cast<int>(line.find("//",0));
        if(poscom != -1)
          {
          line = line.substr(0,poscom);
          }
        poscom = static_cast<int>(line.find("/*",0));
        while(poscom != -1)
          {
          int poscomend = static_cast<int>(line.find("*/",0));
          if(poscomend == -1)
            {
            line = line.substr(0,poscom);
            break;
            }

          std::string line_temp = line.substr(0,poscom);
          line = line_temp+line.substr(poscomend+2,line.size()-poscomend-2);
          poscom = static_cast<int>(line.find("/*",poscom+1));
          }

        // If we have any '(' in the line we stop
        if( !this->IsBetweenChars( '(', ')', posType, false ) &&
          !this->IsBetweenChars( '<', '>', posType, false ))
          {
          // This is a very simple check we count the number of comas
          unsigned int vars = 1;
          pos = static_cast<long int>(line.find(',',0));
          while(pos!=-1)
            {
            posType = m_BufferNoComment.find(',', posType+1);
            if( !this->IsBetweenCharsFast( '(', ')', posType, false ) &&
              !this->IsBetweenCharsFast( '<', '>', posType, false ))
              {
              // Check that we are not initializing an array
              bool betweenBraces = false;
              long int openCurly = pos-1;
              while(openCurly>0)
                {
                // Ok we have the opening
                if(line[openCurly] == '{')
                  {
                  long int posClosing = static_cast<long int>(this->FindClosingChar('{','}',openCurly,false,line));
                  if(posClosing == -1
                    || pos<posClosing)
                    {
                    betweenBraces = true;
                    }
                  break;
                  }
                openCurly--;
                }
              // Check if we are not at the end of the line
              bool endofline = true;
              int eof = pos+1;
              while(eof < (int)line.size())
                {
                if(line[eof] != ' ' && line[eof] != '\n'
                   && line[eof] != '\r' && line[eof] != '\t')
                  {
                  endofline = false;
                  break;
                  }
                eof++;
                }

              if(!betweenBraces && !endofline)
                {
                vars++;
                }
              }
            pos = static_cast<long int>(line.find(',',pos+1));
            }

          if(vars > max)
            {
            Error error;
            error.line = this->GetLineNumber(posType,true);
            error.line2 = error.line;
            error.number = VARIABLEPERLINE;
            error.description = "Number of variable per line exceed: ";
            char* localval = new char[10];
            sprintf(localval,"%d",vars);
            error.description += localval;
            error.description += " (max=";
            delete [] localval;
            localval = new char[10];
            sprintf(localval,"%ld",max);
            error.description += localval;
            error.description += ")";
            delete [] localval;
            m_ErrorList.push_back(error);
            hasError = true;
            }
          }
        }// end firstWord
      posType = m_BufferNoComment.find(typeToFind,posType+1);
      }
    }

  return !hasError;
}

} // end namespace kws
