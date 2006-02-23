/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsParser.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {

/** Constructor */
Parser::Parser()
{
  m_HeaderFilename = "";

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
    this->CheckTypedefs(value);
    return true;
    }
  else if(!strcmp(name,"InternalVariables"))
    {
    this->CheckInternalVariables(value);
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
    std::string v4 = val.substr(pos+1,val.length()-pos-1);

    if(!strcmp(v4.c_str(),"true"))
      {
      this->CheckComments(v1.c_str(),v2.c_str(),v3.c_str(),true);
      }
    else
      {
      this->CheckComments(v1.c_str(),v2.c_str(),v3.c_str(),false);
      }
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
    std::string v1 = val.substr(0,pos);
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

    this->CheckHeader(v1.c_str(),spaceEndOfLine,useCVS); 
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
    this->CheckIfNDefDefine(value);
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
  return ErrorTag[number];
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

void Parser::ConvertBufferToWindowsFileType(std::string & buffer)
{
  long int pos = buffer.find("\n");
  long int pos2 = -1;
  if(pos>1)
    {
    pos2 = buffer.find("\r\n",pos-1); 
    }
  
  while(pos != -1 && (pos-pos2)!=1)
    {
    buffer.insert(pos,"\r");
    pos = buffer.find("\n",pos+2);
    if(pos>1)
      {
      pos2 = buffer.find("\r\n",pos-1);   
      }
    }
  
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

/** Find the previous word given a position */
std::string Parser::FindPreviousWord(long int pos,bool withComments) const
{
  std::string stream = m_BufferNoComment;
  if(withComments)
    {
    stream = m_Buffer;
    }

  long i=pos;

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
  while(i>=0 && inWord)
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
unsigned long Parser::GetPositionInLine(long pos)
{
  long begin = pos;
  while(begin>0 && m_BufferNoComment[begin]!='\n')
    {
    begin--;
    }
  return pos-begin;
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

    // We extract the complete definition.
    // This means that we look for a '{' or '}' or '{' 
    while(i>=0)
      {
      if((m_BufferNoComment[i] == '{')
        || (m_BufferNoComment[i] == '}')
        || (m_BufferNoComment[i] == '{')
        )
        {
        break;
        }
      i--;
      }

    std::string subphrase = "";
    if(i>=0)
      {
      subphrase = m_BufferNoComment.substr(i+1,posSemicolon-i-1);
      }

    if( (subphrase.find("=") == -1)
      && (subphrase.find("(") == -1)
      && (subphrase.find("typedef") == -1)
      )
      {
      return ivar;
      }

    // We find the words until we find a semicolon
    /*long int p = pos;
    std::string pword = this->FindPreviousWord(p);
    bool isTypedef = false;
    while((pword.size()>0) && (pword.find(";") == -1) && (p>0))
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
      }*/
    }

  pos = -1;
  return "";
}

/** Given the position without comments return the position with the comments */
long int Parser::GetPositionWithComments(long int pos) const
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

/** Return the line number in the source code given the character position */
long int Parser::GetLineNumber(long int pos,bool withoutComments) const
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

/** Find public area in source code. */
void Parser::FindPublicArea(long &before, long &after, size_t startPos) const
{
  const long maxchar = 99999999;
  before = 0;
  after = 0;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public", startPos);
  long priv = m_BufferNoComment.find("private", startPos);
  long protect = m_BufferNoComment.find("protected", startPos);

  if(priv == -1)
    {
    priv = maxchar;
    }

  if(protect == -1)
    {
    protect = maxchar;
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

  // If there is nothing after we point to the end of the class
  if(after == maxchar)
    {
    long int classpos = this->GetClassPosition(startPos);
    if(classpos != -1)
      {
      long int posBrace = m_BufferNoComment.find("{",classpos);          
      if(posBrace != -1)
        {
        long int end = this->FindClosingChar('{','}',posBrace,true);
        if(end != -1)
          {
          after = end;
          }
        }
      }
    }
}


/** Find protected area in source code. */
void Parser::FindProtectedArea(long &before, long &after, size_t startPos) const
{
  before = 0;
  after = 0;
  const long maxchar = 99999999;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public", startPos);
  long priv = m_BufferNoComment.find("private", startPos);
  long protect = m_BufferNoComment.find("protected", startPos);

  if(priv == -1)
    {
    priv = maxchar;
    }

  if(pub == -1)
    {
    pub = maxchar;
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


  // If there is nothing after we point to the end of the class
  if(after == maxchar)
    {
    long int classpos = this->GetClassPosition(startPos);
    if(classpos != -1)
      {
      long int posBrace = m_BufferNoComment.find("{",classpos);          
      if(posBrace != -1)
        {
        long int end = this->FindClosingChar('{','}',posBrace,true);
        if(end != -1)
          {
          after = end;
          }
        }
      }
    }
}

/** Find private area in source code. */
void Parser::FindPrivateArea(long &before, long &after, size_t startPos) const
{
  before = 0;
  after = 0;
  const long maxchar = 99999999;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public", startPos);
  long priv = m_BufferNoComment.find("private", startPos);
  long protect = m_BufferNoComment.find("protected", startPos);

  if(pub == -1)
    {
    pub = maxchar;
    }

  if(protect == -1)
    {
    protect = maxchar;
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

  // If there is nothing after we point to the end of the class
  if(after == maxchar)
    {
    long int classpos = this->GetClassPosition(startPos);
    if(classpos != -1)
      {
      long int posBrace = m_BufferNoComment.find("{",classpos);          
      if(posBrace != -1)
        {
        long int end = this->FindClosingChar('{','}',posBrace,true);
        if(end != -1)
          {
          after = end;
          }
        }
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
bool Parser::IsBetweenCharsFast(const char begin, const char end ,long int pos,bool withComments) const
{
  std::string stream = m_BufferNoComment;
  if(withComments)
    {
    stream = m_Buffer;
    }

  if(pos == -1)
    {
    return false;
    }

  long int b0 = stream.find(begin,0);
  long int b1 = stream.find(end,b0);

  while(b0 != -1 && b1 != -1 && b1>b0)
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
bool Parser::IsBetweenChars(const char begin, const char end ,long int pos,bool withComments) const
{
  std::string stream = m_BufferNoComment;
  if(withComments)
    {
    stream = m_Buffer;
    }

  if(pos == -1)
    {
    return false;
    }

  long int b0 = stream.find(begin,0);
  long int b1 = this->FindClosingChar(begin,end,b0,!withComments);

  while(b0 != -1 && b1 != -1 && b1>b0)
    {
    if(pos>b0 && pos<b1)
      {
      return true;
      }
    b0 = stream.find(begin,b0+1);
    b1 = this->FindClosingChar(begin,end,b0,!withComments);
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
  long firstNC = m_BufferNoComment.find("/*",0);
  long lastNC = m_BufferNoComment.find("*/",firstNC);
  long first = m_Buffer.find("/*",0);
  long last = m_Buffer.find("*/",first);

  while((lastNC>firstNC) && (firstNC!= -1) && (lastNC !=-1))
    {
    PairType pair(first,last+2);
    m_CommentPositions.push_back(pair);
    //offset += last+2-first;
    m_BufferNoComment = m_BufferNoComment.erase(firstNC,lastNC+2-firstNC);
    count += lastNC+2-firstNC;
    firstNC = m_BufferNoComment.find("/*",0);
    lastNC = m_BufferNoComment.find("*/",firstNC);
    first = m_Buffer.find("/*",last+1);
    last = m_Buffer.find("*/",first);
    };

  // Then the // comments
  firstNC = m_BufferNoComment.find("//",0);
  lastNC = m_BufferNoComment.find("\n",firstNC);
 
  while((lastNC>firstNC) && (firstNC!= -1) && (lastNC !=-1))
    {
    first = this->GetPositionWithComments(firstNC);
    last = this->GetPositionWithComments(lastNC);

    PairType pair(first,last+1);
    
    // we insert at the right place
    std::vector<PairType>::iterator it = m_CommentPositions.begin();
    while(it != m_CommentPositions.end())
      {
      if((*it).first>first)
        {
        m_CommentPositions.insert(it,pair);
        break;
        }
      it++;
      }
    if(it == m_CommentPositions.end())
      {
      m_CommentPositions.push_back(pair);
      }

    m_BufferNoComment = m_BufferNoComment.erase(firstNC,lastNC+1-firstNC);
    count += lastNC+1-firstNC;
    firstNC = m_BufferNoComment.find("//",0);
    lastNC = m_BufferNoComment.find("\n",firstNC);
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
long int Parser::FindClosingChar(char openChar, char closeChar, long int pos,bool noComment) const
{  
  std::string stream = m_Buffer.c_str();
  if(noComment)
    {
    stream = m_BufferNoComment.c_str();
    }

  long int open = 1;
  for(size_t i=pos+1;i<stream.length();i++)
    { 
    if(stream[i] == openChar)
      {
      open++;
      }
    else if(stream[i] == closeChar)
      {
      open--;
      }
    if(open == 0)
      {
      return (long int)i;
      }
    }
  return -1; // closing char not found
}

} // end namespace kws
