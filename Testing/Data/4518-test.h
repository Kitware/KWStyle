class Base
{
   public:
      void blub();a
};

class Test:public Base
{
   public:
      Test():Base(5),m_i(0) {}
      Base &getBase();
      friend Base::blub();
      void bar();
};

