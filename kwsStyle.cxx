/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsStyle.cxx
  Author:    Julien Jomier

  Copyright (c) Kitware, Inc., Insight Consortium.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsParser.h"
#include "kwsGenerator.h"
#include "metaCommand.h"
#include <iostream>
#include <fstream>
#include <itksys/Directory.hxx>

#if defined(WIN32) && !defined(__CYGWIN__)
  #include <direct.h> // mkdir needs it
#endif

int main(int argc, char **argv)
{ 
  MetaCommand command;

  command.SetOption("directory","d",false,"Specify a directory");
  command.SetOption("html","html",false,"Generate the HTML report");
  command.AddOptionField("html","filename",MetaCommand::STRING,false);

  //command.AddOptionField("directory","filename",MetaCommand::STRING,true);
  //command.SetOptionComplete("directory",true);
  command.AddField("infile","input filename",MetaCommand::STRING,true);

  // Parsing
  if(!command.Parse(argc,argv))
    {
    return 1;
    }

  std::string inputFilename = command.GetValueAsString("infile");

  bool parseDirectory = false;
  if(command.GetValueAsString("directory").length() > 0)
    {
    parseDirectory = true;
    if(inputFilename[inputFilename.size()-1] != '/')
      {
      inputFilename += '/';
      }
    }
  
  //std::string inputFilename = "C:/Julien/Workspace/Insight/Code/Common/itkLog10ImageAdaptor.h";
  
  std::vector<std::string> filenames;

  std::vector<kws::Parser> m_Parsers;

  if(parseDirectory)
    {
    itksys::Directory directory;
    directory.Load(inputFilename.c_str());
    for(unsigned int i=0;i<directory.GetNumberOfFiles();i++)
      {
      std::string file = directory.GetFile(i);
      if((file.find(".h") != -1)
         || (file.find(".cxx") != -1)
         || (file.find(".txx") != -1)
         )
        {
        filenames.push_back(inputFilename+file);
        }
      }
    }
  else
    {
    filenames.push_back(inputFilename);
    }

  std::vector<std::string>::const_iterator it = filenames.begin();

  unsigned long errors =0;

  while(it != filenames.end())
    {
    //std::cout << "Input File = " << (*it).c_str() << std::endl;

    // We open the file
    std::ifstream file;
    file.open((*it).c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open file: " << (*it).c_str() << std::endl;
      return 0;
      }

    file.seekg(0,std::ios::end);
    unsigned long fileSize = file.tellg();
    file.seekg(0,std::ios::beg);

    char* buf = new char[fileSize+1];
    file.read(buf,fileSize);
    buf[fileSize] = 0; 
    std::string buffer(buf);
    buffer.resize(fileSize);
    delete [] buf;
   
    file.close();

    kws::Parser parser;
    parser.SetFilename((*it).c_str());
    parser.SetBuffer(buffer);
    parser.CheckLineLength(77); // this is required
    //parser.ClearErrors();
    parser.CheckDeclarationOrder(0,2,1);
    parser.CheckInternalVariables("m_[A-Z]");
    parser.CheckSemicolonSpace(2);
    parser.CheckEndOfFileNewLine();
    parser.CheckTabs();
    parser.CheckIndent(kws::SPACE,2);
    parser.CheckHeader("c:/Julien/Workspace/KWStyle/kwsHeader.h",false);
    //  {
    //  std::cout << (*it).c_str() << " Header error" << std::endl;
    //  errors++;
    //  }
/*
    if(!parser.CheckIfNDefDefine("_<NameOfClass>_<Extension>"))
      {
      std::cout << (*it).c_str() << std::endl; 
      std::cout << parser.GetLastErrors().c_str() << std::endl;
      errors++;
      }
    //std::cout << parser.GetInfo() << std::endl;
*/

    m_Parsers.push_back(parser);
    it++;
    } // end filenames  

  //std::cout << "Errors = " << errors << std::endl;

  // If we should generate the HTML file
  if(command.GetOptionWasSet("html"))
    {
    std::string html = command.GetValueAsString("html","filename");
    kws::Generator generator;
    generator.SetParser(&m_Parsers);
    generator.GenerateHTML(html.c_str());
    }

  return 1;
}
