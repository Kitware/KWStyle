/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckDeclarationOrder.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"

namespace kws {


/** Check the order of the declaration */
bool Parser::CheckDeclarationOrder(unsigned int posPublic, unsigned int posProtected, unsigned int posPrivate)
{
  m_TestsDone[DECL_ORDER] = true;
  char* val = new char[255];
  sprintf(val,"Declaration order should match ");
  m_TestsDescription[DECL_ORDER] = val;

  if(posPublic>posProtected && posProtected>posPrivate)
    {
    m_TestsDescription[DECL_ORDER] += "Private/Protected/Public";
    }
  else if(posPublic<posProtected && posProtected<posPrivate)
    {
    m_TestsDescription[DECL_ORDER] += "Public/Protected/Private";
    }
  else if(posPublic>posPrivate && posProtected<posPrivate)
    {
    m_TestsDescription[DECL_ORDER] += "Protected/Private/Public";
    } 
  else if(posPublic<posPrivate && posProtected<posPrivate)
    {
    m_TestsDescription[DECL_ORDER] += "Protected/Public/Private";
    }

  delete [] val;

  long int publicFirst;
  long int publicLast;
  this->FindPublicArea(publicFirst,publicLast);
  
  long int class_end = this->FindEndOfClass(publicFirst);
  long int class_beg = this->FindOpeningChar('}','{',class_end,true);

  // Find the first public declaration
  while(publicFirst!=-1 && publicFirst!=MAX_CHAR && (publicFirst<class_beg || publicFirst>class_end))
    {
    this->FindPublicArea(publicFirst,publicLast,publicFirst+1);
    class_end = this->FindEndOfClass(publicFirst);
    class_beg = this->FindOpeningChar('}','{',class_end,true);
    }
  
  // Currently checking only one class per file (do a loop in the future)
  long int currentclass = class_beg;

  long int protectedFirst;
  long int protectedLast;
  this->FindProtectedArea(protectedFirst,protectedLast);

  class_end = this->FindEndOfClass(protectedFirst);
  class_beg = this->FindOpeningChar('}','{',class_end,true);
  while(protectedFirst!=-1 && protectedFirst!=MAX_CHAR &&class_beg != currentclass)
    {
    this->FindProtectedArea(protectedFirst,protectedLast,protectedFirst+1);
    class_end = this->FindEndOfClass(protectedFirst);
    class_beg = this->FindOpeningChar('}','{',class_end,true);
    }
  
  long int privateFirst;
  long int privateLast;
  this->FindPrivateArea(privateFirst,privateLast);

  class_end = this->FindEndOfClass(privateFirst);
  class_beg = this->FindOpeningChar('}','{',class_end,true);

  while(privateFirst!=-1 && privateFirst!=MAX_CHAR && class_beg != currentclass)
    {
    this->FindPrivateArea(privateFirst,privateLast,privateFirst+1);    
    class_end = this->FindEndOfClass(privateFirst);
    class_beg = this->FindOpeningChar('}','{',class_end,true);
    }
 
  bool hasError = false;

  // public v.s protected
  if( (posPublic > posProtected)
      && (protectedFirst != MAX_CHAR)
      && (publicFirst != MAX_CHAR)
      && (protectedFirst > publicFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(publicFirst,true);
    error.line2 = this->GetLineNumber(publicFirst,true);
    error.number = DECL_ORDER;
    error.description = "Public defined before Protected";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPublic < posProtected)
      && (protectedFirst != MAX_CHAR)
      && (publicFirst != MAX_CHAR)
      && (protectedFirst < publicFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(protectedFirst,true);
     error.line2 = this->GetLineNumber(protectedFirst,true);
     error.number = DECL_ORDER;
     error.description = "Protected defined before Public";
     m_ErrorList.push_back(error);
     hasError = true;
     }

  // protected v.s. private
  if( (posPrivate > posProtected)
      && (protectedFirst != MAX_CHAR)
      && (privateFirst != MAX_CHAR)
      && (protectedFirst > privateFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(privateFirst,true);
    error.line2 = this->GetLineNumber(privateFirst,true);
    error.number = DECL_ORDER;
    error.description = "Private defined before Protected";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPrivate > posProtected)
      && (protectedFirst != MAX_CHAR)
      && (privateFirst != MAX_CHAR)
      && (protectedFirst > privateFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(protectedFirst,true);
     error.line2 = this->GetLineNumber(protectedFirst,true);
     error.number = DECL_ORDER;
     error.description = "Protected defined before Private";
     m_ErrorList.push_back(error);
     hasError = true;
     }

  // Public v.s. Private
  if( (posPublic > posPrivate)
      && (privateFirst != MAX_CHAR)
      && (publicFirst != MAX_CHAR)
      && (privateFirst > publicFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(publicFirst,true);
    error.line2 = this->GetLineNumber(publicFirst,true);
    error.number = DECL_ORDER;
    error.description = "Public defined before Private";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPublic > posPrivate)
      && (privateFirst != MAX_CHAR)
      && (publicFirst != MAX_CHAR)
      && (privateFirst > publicFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(privateFirst,true);
     error.line2 = this->GetLineNumber(privateFirst,true);
     error.number = DECL_ORDER;
     error.description = "Private defined before Public";
     m_ErrorList.push_back(error);
     hasError = true;
     }
/*
   // Print the info
   Info info;
   info.line =  this->GetLineNumber(protectedFirst,true);
   info.line2 = this->GetLineNumber(protectedFirst,true);
   info.number = DECL_ORDER;
   if(publicFirst == MAX_CHAR)
     {
     info.description = "This class doesn't have any public method";
     m_InfoList.push_back(info);
     }
   if(protectedFirst == MAX_CHAR)
     {
     info.description = "This class doesn't have any protected method";
     m_InfoList.push_back(info);
     }
   if(privateFirst == MAX_CHAR)
     {
     info.description = "This class doesn't have any private method";
     m_InfoList.push_back(info);
     }
  */ 
  return !hasError;
}

} // end namespace kws
