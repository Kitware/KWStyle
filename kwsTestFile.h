/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsTestFile.h

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef kwsTestFile_h
#define kwsTestFile_h

#include "itkXMLDefs.h"

namespace kws
{

// Forward of internally used class.
class XMLDescription;

/** this is a test
 *  for the comments */
class itkXMLReader
{
public:

  //! Read an XML file and populate the current XML description. 
  //! Return 1 on success, otherwise 0.
  static int Read(const std::string &filename, XMLDescription &desc, bool suppressError=true)
   {
  }

m_testouille   ;
private:

  //! Parse the buffer starting from position init for the xml tag "tag", 
  //! return its content as string value and set init to the position 
  //! directly after the end of the parsed tag.
  static std::string ReadTag(std::string buffer, const std::string &tag, size_t & init) {  ;}

protected:

};

} // end namespace

#endif



  
 
 

 
