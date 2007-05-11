#ifndef TEST_H
#define TEST_H

struct Event
{
   protected:
};

class CommThread
{
   public:
      CommThread();
   protected:
      Data * const m_data;
      Comm m_comm;
};


#endif
