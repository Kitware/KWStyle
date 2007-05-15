/*
  
*/

#ifndef _Test_H_
#define _Test_H_

class Test
{

  public    :

    enum
    {
      eNoJob      = 0 ,
      eValidJob   = 1 ,
      eJobRunning = 2 ,
      eJobStopped = 3 ,
      eJobAborted = 4 ,
      eJobDone    = 5 ,
      eJobFailed  = 6
    };

    enum
    {
      eAllSubActions     = 0 ,
      ePendingSubActions = 1 ,
      eSubActionsDone    = 2 
    };
  
    //! operator functions
    virtual Test & operator +=( TKType iSubAction ) = 0;
    virtual Test & operator -=( TKType iSubAction ) = 0;

    //! job state
    virtual const void * GetData ( void       ) const = 0;
    virtual int          GetState( void       ) const = 0;
    virtual int          SetState( int iState )       = 0;

    //! jobs subactions list
    virtual TKList<TKType> GetSubActions( int iSelector = ePendingSubActions                 ) const = 0;
    virtual unsigned       Count        ( TKType iSubAction = 0                              ) const = 0;
    virtual TKType         Get          ( unsigned iIndex = 0                                ) const = 0;
    virtual Test &      Append       ( TKType iSubAction                                  )       = 0;
    virtual Test &      Prepend      ( TKType iSubAction                                  )       = 0;
    virtual Test &      Insert       ( TKType iSubAction , unsigned iIndex                )       = 0;
    virtual Test &      Delete       ( unsigned iIndex = 0                                )       = 0;
    virtual Test &      Delete       ( TKType iSubAction , unsigned iIndex = (unsigned)-1 )       = 0;
    
    //! set completed
    virtual void Completed( void ) = 0;

  protected :
  
    //! dtor
    virtual ~Test( void ) {};
  
  private   :

};//class Test

#endif//_Test_H_
