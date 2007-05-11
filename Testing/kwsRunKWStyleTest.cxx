/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    kwsRunKWStyleTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include <iostream>
#include <kwssys/Process.h>

int kwsRunKWStyleTest(int argc, char* argv[] )
{
  argv++;

  // Run KWSTyle
  kwssysProcess* gp = kwssysProcess_New();
  kwssysProcess_SetCommand(gp,argv);
  kwssysProcess_Execute(gp);

  kwssysProcess_WaitForExit(gp,0);

  int result = 1;
  switch(kwssysProcess_GetState(gp))
    {
    case kwssysProcess_State_Exited:
      {
      result = kwssysProcess_GetExitValue(gp);
      } break;
    case kwssysProcess_State_Error:
      {
      std::cerr << "Error: Could not run " << argv[0] << ":\n";
      std::cerr << kwssysProcess_GetErrorString(gp) << "\n";
      } break;
    case kwssysProcess_State_Exception:
      {
      std::cerr << "Error: " << argv[0]
                << " terminated with an exception: "
                << kwssysProcess_GetExceptionString(gp) << "\n";
      } break;
    case kwssysProcess_State_Starting:
    case kwssysProcess_State_Executing:
    case kwssysProcess_State_Expired:
    case kwssysProcess_State_Killed:
      {
      // Should not get here.
      std::cerr << "Unexpected ending state after running " << argv[0]
                << std::endl;
      } break;
    }
  kwssysProcess_Delete(gp);

  return 0;
}
