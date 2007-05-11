class Test : public Base {
   public:

      Test();
      Test():Base()             {}
      Test()             {m_foo=0;}
      Test():Base()             {m_foo=0;}
      ~Test();
      ~Test()                   {m_foo=1;}
};

