/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsCheckDeclarationOrder.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
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
  
  long int protectedFirst;
  long int protectedLast;
  this->FindProtectedArea(protectedFirst,protectedLast);
  
  long int privateFirst;
  long int privateLast;
  this->FindPrivateArea(privateFirst,privateLast);
 
  bool hasError = false;

  // public v.s protected
  if( (posPublic > posProtected)
      && (posPublic != -1)
      && (posProtected != -1)
      && (protectedFirst > publicFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(protectedFirst,true);
    error.line2 = this->GetLineNumber(publicFirst,true);
    error.number = DECL_ORDER;
    error.description = "Public defined before Protected";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPublic > posProtected)
      && (posPublic != -1)
      && (posProtected != -1)
      && (protectedFirst > publicFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(protectedFirst,true);
     error.line2 = this->GetLineNumber(publicFirst,true);
     error.number = DECL_ORDER;
     error.description = "Protected defined before Public";
     m_ErrorList.push_back(error);
     hasError = true;
     }

  // protected v.s. private 
  if( (posPrivate > posProtected)
      && (posPrivate != -1)
      && (posProtected != -1)
      && (protectedFirst > privateFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(protectedFirst,true);
    error.line2 = this->GetLineNumber(privateFirst,true);
    error.number = DECL_ORDER;
    error.description = "Private defined before Protected";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPrivate > posProtected)
      && (posPrivate != -1)
      && (posProtected != -1)
      && (protectedFirst > privateFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(protectedFirst,true);
     error.line2 = this->GetLineNumber(privateFirst,true);
     error.number = DECL_ORDER;
     error.description = "Protected defined before Private";
     m_ErrorList.push_back(error);
     hasError = true;
     }

  // Public v.s. Private
  if( (posPublic > posPrivate)
      && (posPublic != -1)
      && (posPrivate != -1)
      && (privateFirst > publicFirst)
    ) 
    {
    Error error;
    error.line =  this->GetLineNumber(privateFirst,true);
    error.line2 = this->GetLineNumber(publicFirst,true);
    error.number = DECL_ORDER;
    error.description = "Public defined before Private";
    m_ErrorList.push_back(error);
    hasError = true;
    }
  else if(
     (posPublic > posPrivate)
      && (posPublic != -1)
      && (posPrivate != -1)
      && (privateFirst > publicFirst)
      )
     {
     Error error;
     error.line =  this->GetLineNumber(privateFirst,true);
     error.line2 = this->GetLineNumber(publicFirst,true);
     error.number = DECL_ORDER;
     error.description = "Private defined before Public";
     m_ErrorList.push_back(error);
     hasError = true;
     }

   // Print the info
   Info info;
   info.line =  this->GetLineNumber(protectedFirst,true);
   info.line2 = this->GetLineNumber(publicFirst,true);
   info.number = DECL_ORDER;
   if(publicFirst == -1)
     {
     info.description = "This class doesn't have any public method";
     m_InfoList.push_back(info);
     }
   if(protectedFirst == -1)
     {
     info.description = "This class doesn't have any protected method";
     m_InfoList.push_back(info);
     }
   if(privateFirst == -1)
     {
     info.description = "This class doesn't have any private method";
     m_InfoList.push_back(info);
     }
   
  return !hasError;
}

} // end namespace kws
