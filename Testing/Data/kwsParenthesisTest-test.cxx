int main(argc,argv) // This line has no spaces
{
  foo( i, j, k )  // This line has one spaces
  foo(           // This should always be ok
    i, j, k )  // This line has one spaces
  foo(i,  j,  k)  // This line has no spaces
  foo()  // This is always ok
  foo( (i) ) // Items in paren in paren are ignored
  foo( ( i ) ) // Items in paren in paren are ignored
  foo(( i )) // This is always ok
  foo((i)) // This is always ok
}
