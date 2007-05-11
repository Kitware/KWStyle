        void  FreeMyMsg(COMMAND* pMsg)
        { cyg_scheduler_lock();if(pMsg!=NULL)m_MyMsgPool.free(pMsg);cyg_scheduler_unlock(); }  // m_MyMsgPool.alloc() in dsr
        //----------------------------------
        
