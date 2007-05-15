#include "Test.h"

template <class T>
Test<T>::Test
( 
T &                 iObject               ,
void (T::*          iAlloc)( HCBuffer & ) ,
void (T::*          iFree )( HCBuffer & ) ,
size_t              iSize                 , test,
int                 iIsLocked             , 
int                 iSeverity             , 
int                 iIsFIFO               ,
TestRestoreIF * iMemoryRestore
)
: Test( kTypeAlienMemory , iIsLocked , iSeverity , iIsFIFO , 0 , iMemoryRestore )
, mObject ( iObject                                                                 )
, mAlloc  ( iAlloc                                                                  )
, mFree   ( iFree                                                                   )
, mIsOwner( 1                                                                       )
{
 // optional allocate buffer
 if( iSize )
   Alloc( iSize );
}
