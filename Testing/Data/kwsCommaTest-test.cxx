int main(argc,argv) // This line has no spaces
{
  int i, j;  // This line has one space
  string s="test,,,me";  // This line should be skipped
  // test,,,me           // This line should also be skipped
  string s="test, , , me"; // This line should also also be skipped
  int test, me,  two;  // This line has one and two spaces
  foo(i, j, k)  // This line has one spaces
  foo(i,     // This should always be ok
    j, k)  // This line has one spaces
  foo(i,  j,  k)  // This line has two spaces
}
