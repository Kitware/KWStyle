#ifndef _HCDEBUG_H_
#define _HCDEBUG_H_

class HCDebug
{
  public    :
  
    //! ctor/dtor
    HCDebug( const TKString & iClass , const TKString & iName = TKString() );
    virtual ~HCDebug( void );

  protected :

    struct Global
    {
      TKString mCommand;
      TKString mHelp;
      void (*  tProc)( const TKList<TKString> & iList );
    };

    template <class T>
    struct Command
    {
      TKString  mCommand;
      TKString  mHelp;
      void (T::*mProc)( const TKList<TKString> & iList );
    };

    //! static class vars
    static TKList<Global           > sGlobals;
    static TKList<Command<HCDebug> > sCommands;
    static TKList<HCDebug*         > sInstances;
    static TKCriticalSection        sInstancesCS;

    //! data members
    TKString mClass;
    TKString mName;

    //! objects TSM-Handler
    virtual int DoHandleTSM( const TKList<TKString> & iList );

    template <class T>
    int FindCommand( const TKList<TKString> & iList , const TKList<Command<T> > & iCommands );


};//class HCDebug


#endif//_HCDEBUG_H_
