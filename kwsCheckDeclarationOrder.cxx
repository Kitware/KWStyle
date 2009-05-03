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
bool Parser::CheckDeclarationOrder(size_t posPublic, size_t posProtected, size_t posPrivate)
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

  size_t publicFirst;
  size_t publicLast;
  this->FindPublicArea(publicFirst,publicLast);
  
  size_t class_end = this->FindEndOfClass(publicFirst);
  size_t class_beg = this->FindOpeningChar('}','{',class_end,true);

  // Find the first public declaration
  while(publicFirst!=std::string::npos && publicFirst!=MAX_CHAR && (publicFirst<class_beg || publicFirst>class_end))
    {
    this->FindPublicArea(publicFirst,publicLast,publicFirst+1);
    class_end = this->FindEndOfClass(publicFirst);
    class_beg = this->FindOpeningChar('}','{',class_end,true);
    }
  
  // Currently checking only one class per file (do a loop in the future)
  size_t currentclass = class_beg;

  size_t protectedFirst;
  size_t protectedLast;
  this->FindProtectedArea(protectedFirst,protectedLast);

  class_end = this->FindEndOfClass(protectedFirst);
  class_beg = this->FindOpeningChar('}','{',class_end,true);
  while(protectedFirst!=std::string::npos && protectedFirst!=MAX_CHAR &&class_beg != currentclass)
    {
    this->FindProtectedArea(protectedFirst,protectedLast,protectedFirst+1);
    class_end = this->FindEndOfClass(protectedFirst);
    class_beg = this->FindOpeningChar('}','{',class_end,true);
    }
  
  size_t privateFirst;
  size_t privateLast;
  this->FindPrivateArea(privateFirst,privateLast);

  class_end = this->FindEndOfClass(privateFirst);
  class_beg = this->FindOpeningChar('}','{',class_end,true);

  while(privateFirst!=std::string::npos && privateFirst!=MAX_CHAR && class_beg != currentclass)
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
     
  return !hasError;
}

} // end namespace kws
