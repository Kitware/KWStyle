    Type currentFileType() const           {if (m_currentFile==0) return File::Type_Dummy; return m_currentFile->type();}

    FileInfo* m_currentFileInfo;           ///< ------------------------------------------------------------------------------------
    FileInfo m_tmpFileInfo;                ///< ---------------------------------------------------, ----
    FileManager m_fileManager;
    int m_currentFileIndex;                ///< ----------------------------------------------

