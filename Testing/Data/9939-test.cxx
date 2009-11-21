/*=========================================================================

  Program:   Semicolon Bug

=========================================================================*/

namespace
{
// Blah blah
}; // Unnecessary semicolon should be reported at line 10 not line 9

int main( int argc, char *argv[] )
{
  return 0;
}
