/*=========================================================================

  Program:   ITKXML
  Module:    kwsTestFile.h
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
#ifndef __itkXMLReader_H
#define __itkXMLReader_H

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
  static std::string ReadTag(std::string buffer, const std::string &tag, long int & init) {  ;}

protected:

};

} // end namespace

#endif



  
 
 

 
