struct Event
{
   CON_COMMAND*   GetMsg()    {return helper()->pMsg;  }

   protected:
      Helper* helper(void) {return (Helper*)data;}
};

class Thread
{
   public:
      Thread(Imprint& Data);
};
