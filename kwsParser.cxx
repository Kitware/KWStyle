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

#include <kwssys/ios/sstream>

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
  else if(!strcmp(name,"Spaces"))
    {
    this->CheckExtraSpaces(atoi(value));
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
  size_t cc;
  const char* inch = buffer.c_str();
  std::vector<char> outBuffer;
  size_t inStrSize = buffer.size();
  // Reserve enough space for most files
  outBuffer.reserve(inStrSize+1000);

  for ( cc = 0; cc < inStrSize; ++ cc )
    {
    if ( *inch == '\n' )
      {
      if ( cc == 0 || *(inch-1) != '\r' )
        {
        outBuffer.push_back('\r');
        }
      }
    outBuffer.push_back(*inch);
    inch ++;
    }
  outBuffer.push_back(0);
  buffer = &*outBuffer.begin();
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
  before = 0;
  after = 0;

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public:", startPos);
  long priv = m_BufferNoComment.find("private:", startPos);
  long protect = m_BufferNoComment.find("protected:", startPos);
  
  if(pub == -1)
    {
    pub = MAX_CHAR;
    }

  if(priv == -1)
    {
    priv = MAX_CHAR;
    }

  if(protect == -1)
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
  if(after == MAX_CHAR)
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

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public:", startPos);
  long priv = m_BufferNoComment.find("private:", startPos);
  long protect = m_BufferNoComment.find("protected:", startPos);

  if(priv == -1)
    {
    priv = MAX_CHAR;
    }
  
  if(protect == -1)
    {
    protect = MAX_CHAR;
    }

  if(pub == -1)
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
  if(after == MAX_CHAR)
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

  // First look if public is before protected and private
  long pub = m_BufferNoComment.find("public:", startPos);
  long priv = m_BufferNoComment.find("private:", startPos);
  long protect = m_BufferNoComment.find("protected:", startPos);
  
  if(priv == -1)
    {
    priv = MAX_CHAR;
    }

  if(pub == -1)
    {
    pub = MAX_CHAR;
    }

  if(protect == -1)
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
  if(after == MAX_CHAR)
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


bool Parser::IsInAnyComments(long int pos) const
{
  //Check //
  long int posslash = m_Buffer.find("//",0);
  long int posend = -1;
  if(posslash != -1)
    {
    posend = m_Buffer.find("\n",posslash);
    }

  while(posend != -1)
    {
    if(pos<posend && pos>posslash)
      {
      return true;
      }
    posslash = m_Buffer.find("//",posend+1);
    if(posslash != -1)
      {
      posend = m_Buffer.find("\n",posslash);
      }
    else
      {
      posend = -1;
      }
    }

  // Check the /* */
  long int posstart = m_Buffer.find("/*",0);
  posend = -1;
  if(posstart != -1)
    {
    posend = m_Buffer.find("*/",posstart);
    }

  while(posend != -1)
    {
    if(pos<posend && pos>posstart)
      {
      return true;
      }
    posstart = m_Buffer.find("/*",posend+1);
    if(posstart != -1)
      {
      posend = m_Buffer.find("*/",posstart);
      }
    else
      {
      posend = -1;
      }
    }


  return false;
     
}


bool Parser::IsValidQuote(std::string & stream,long int pos) const
{
  // We need to count the number of \ if it's an odd number
  // then this is not a valid "
  int n = 0;
  long int i = pos-1;
  while(i>0 && stream[i]=='\\')
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
bool Parser::IsBetweenQuote(long int pos,bool withComments) const
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

  // We don't want to check for \" otherwise it fails
  long int b0 = stream.find('"',0);
  while((b0!=-1) && !this->IsValidQuote(stream,b0))
    {
    b0 = stream.find('"',b0+1);
    }

  long int b1 = stream.find('"',b0+1);
  while((b1!=-1) && !this->IsValidQuote(stream,b1))
    {
    b1 = stream.find('"',b1+1);
    }

  while(b0 != -1 && b1 != -1 && b1>b0)
    {
    if(pos>b0 && pos<b1)
      {
      return true;
      }
    b0 = stream.find('"',b1+1);
    while((b0!=-1) && !this->IsValidQuote(stream,b0))
      {
      b0 = stream.find('"',b0+1);
      }

    b1 = stream.find('"',b0+1);
    while((b1!=-1) && !this->IsValidQuote(stream,b1))
      {
      b1 = stream.find('"',b1+1);
      }
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

/** Remove the comments */
#if 1
void Parser::RemoveComments()
{
  m_CommentPositions.clear();
  size_t cc;
  const char* inch = m_Buffer.c_str();
  std::vector<char> outBuffer;
  size_t inStrSize = m_Buffer.size();
  // Reserve enough space for all files. The size will be at least the size of
  // the file.
  outBuffer.reserve(inStrSize+1);

  bool inComment = false;
  bool copyChar;
  bool cppComment = false;

  size_t beginOfComment = 0;
  size_t endOfComment = 0;

  for ( cc = 0; cc < inStrSize; ++ cc )
    {
    copyChar = true;
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
        m_CommentPositions.push_back(pair);
        }
      if ( *inch == '/' )
        {
        if ( cc > 0 && *(inch-1) == '*' )
          {
          endOfComment = cc+1;
          inComment = false;
          copyChar = false;
          PairType pair(beginOfComment, endOfComment);
          m_CommentPositions.push_back(pair);
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
    inch ++;
    }
  outBuffer.push_back(0);
  m_BufferNoComment = &*outBuffer.begin();
}
#endif

#if 0
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
#endif

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
