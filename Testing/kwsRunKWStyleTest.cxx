/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
// Put kwssys/Configure.h first for proper compiler warning suppression:
#include <kwssys/Configure.h>

#include <iostream>
#include <kwssys/Process.h>

int kwsRunKWStyleTest(int , char* argv[] )
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
