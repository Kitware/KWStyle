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
#include <cmath>
#include <sstream>
#include "kwsXMLReader.h"
#include <vector>

#if defined(WIN32) && !defined(__CYGWIN__)
  #include <direct.h> // mkdir needs it
#endif

struct kwsFeature{
  std::string name;
  std::string value;
  bool enable;
  };

// List of features to check
std::vector<kwsFeature> features;

void AddFeature(const char* name,const char* value,bool enable)
{
  kwsFeature f;
  f.name = name;
  f.value = value;
  f.enable = enable;
  features.push_back(f);
}

void ChangeFeature(const char* name,const char* value)
{
  std::vector<kwsFeature>::iterator it = features.begin();
  while(it != features.end())
    {
    if(!strcmp((*it).name.c_str(),name))
      {
      (*it).value = value;
      return;
      }
    it++;
    }
}

void DisableFeature(const char* name)
{
  std::vector<kwsFeature>::iterator it = features.begin();
  while(it != features.end())
    {
    if(!strcmp((*it).name.c_str(),name))
      {
      (*it).enable = false;
      return;
      }
    it++;
    }

}

int main(int argc, char **argv)
{
  MetaCommand command;

  command.SetOption("directory","d",false,"Specify a directory");
  command.SetOption("html","html",false,"Generate the HTML report");
  command.AddOptionField("html","filename",MetaCommand::STRING,false);
  command.SetOption("exporthtml","exporthtml",false,"Export the HTML report online");
  command.SetOption("xml","xml",false,"Read a XML configure file");
  command.AddOptionField("xml","filename",MetaCommand::STRING,false);
  
  command.AddField("infile","input filename",MetaCommand::STRING,true);

  // Parsing
  if(!command.Parse(argc,argv))
    {
    return 1;
    }

  // Add the features
  AddFeature("LineLength","80",true);
  AddFeature("DeclarationOrder","0,1,2",true);
  AddFeature("Typedefs","[A-Z]",true);
  AddFeature("InternalVariables","m_[A-Z]",true);
  AddFeature("SemicolonSpace","0",true);
  AddFeature("EndOfFileNewLine","",true);
  AddFeature("Tabs","",true);
  AddFeature("Comments","/**, *, */,true",true);
  AddFeature("Header","c:/Julien/Workspace/KWStyle/kwsHeader.h,false,true",true);
  AddFeature("Indent","kws::SPACE,2,true,true",true);
  AddFeature("Namespace","itk",true);
  AddFeature("NameOfClass","[NameOfClass],itk",true);
  AddFeature("IfNDefDefine","__[NameOfClass]_[Extension]",true);
  AddFeature("EmptyLines","2",true); 
  AddFeature("Template","T",true); 
  AddFeature("Operator","1,1",true); 

  // If we should generate the HTML file
  if(command.GetOptionWasSet("xml"))
    {
    std::string xml = command.GetValueAsString("xml","filename");
    kws::XMLReader reader;
    reader.Open(xml.c_str());
    
    std::vector<kwsFeature>::iterator it = features.begin();
    while(it != features.end())
      {
      std::string val = reader.GetValue((*it).name.c_str());
      if(val.length() > 0 )
        {
        ChangeFeature((*it).name.c_str(),val.c_str()); 
        }
      else
        {
        DisableFeature((*it).name.c_str());
        }
      it++;
      }
    reader.Close();
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
 
  
  //std::string inputFilename = "C:/Julien/Workspace/Insight/Code/Common/itkPolyLineParametricPath.h";
  //bool parseDirectory = false;

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
         || (file.find(".hxx") != -1)
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
    if(!command.GetOptionWasSet("exporthtml"))
      {
      std::cout << "Input File = " << (*it).c_str() << std::endl;
      }

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

    std::vector<kwsFeature>::iterator itf = features.begin();
    while(itf != features.end())
      {
      if((*itf).enable)
        {
        parser.Check((*itf).name.c_str(),(*itf).value.c_str());
        }
      itf++;
      }

   // parser.CheckLineLength(81); // this is required
/*
    parser.CheckDeclarationOrder(0,1,2);
    parser.CheckTypedefs("[A-Z]");
    //std::cout << parser.GetLastErrors().c_str() << std::endl;
    
    parser.CheckInternalVariables("m_[A-Z]");
    parser.CheckSemicolonSpace(0);
    parser.CheckEndOfFileNewLine();
    parser.CheckTabs();
*/
//    parser.CheckComments("/**"," *"," */",true);
/*
    parser.CheckHeader("c:/Julien/Workspace/KWStyle/kwsHeader.h",false,true); // should be before CheckIndent
    
    //parser.ClearErrors();
    parser.CheckIndent(kws::SPACE,2,true,true);
    //std::cout << parser.GetLastErrors().c_str() << std::endl;  
    
    parser.CheckNamespace("itk");

    parser.CheckNameOfClass("<NameOfClass>","itk");
    parser.CheckIfNDefDefine("__<NameOfClass>_<Extension>");
    parser.CheckEmptyLines(2);
    parser.CheckTemplate("T");
    parser.CheckOperator(1,1);
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

  // If we should export the html report
  if(command.GetOptionWasSet("exporthtml"))
    {
    kws::Generator generator;
    generator.SetParser(&m_Parsers);
    generator.ExportHTML(std::cout);
    }


  return 1;
}
