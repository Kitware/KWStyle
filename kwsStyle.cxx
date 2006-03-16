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
  std::string filename;
  bool enable;
  };

// List of features to check
std::vector<kwsFeature> features;
std::vector<kwsFeature> overwriteFeatures;

void AddFeature(const char* name,const char* value,bool enable)
{
  kwsFeature f;
  f.name = name;
  f.value = value;
  f.enable = enable;
  f.filename = "";
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

/** Push the filenames in the vector */
void AddDirectory(const char* dirname,std::vector<std::string> & filenames,bool recurse = false)
{
  std::cout << "parsing " << dirname << std::endl;
  itksys::Directory directory;
  directory.Load(dirname);
  itksys::Directory dir2;

  for(unsigned int i=0;i<directory.GetNumberOfFiles();i++)
    {
    std::string file = directory.GetFile(i);
    std::string fullpathdir = dirname+file+"/";
    if(recurse && file!=".." && file!="." && dir2.Load(fullpathdir.c_str()))
      {
      AddDirectory(fullpathdir.c_str(),filenames,true);
      }
    else if((file.find(".h") != -1)
       || (file.find(".hxx") != -1)
       || (file.find(".cxx") != -1)
       || (file.find(".txx") != -1)
       )
      {
      filenames.push_back(dirname+file);
      }
    }
}

int main(int argc, char **argv)
{
  double time0 = itksys::SystemTools::GetTime();

  MetaCommand command;

  command.SetOption("directory","d",false,"Specify a directory");
  command.SetOption("recursive","R",false,"Associated with -d recurse through directories");
  command.SetOption("verbose","v",false,"Display errors");
  command.SetOption("quiteverbose","qv",false,"Display less information");
  command.SetOption("lesshtml","lesshtml",false,"Display less HTML");

  command.SetOption("html","html",false,"Generate the HTML report");
  command.AddOptionField("html","filename",MetaCommand::STRING,false);
  command.SetOption("exporthtml","exporthtml",false,"Export the HTML report online");
  command.SetOption("xml","xml",false,"Read a XML configure file");
  command.AddOptionField("xml","filename",MetaCommand::STRING,false);

  command.SetOption("overwrite","o",false,"Overwrite rules file");
  command.AddOptionField("overwrite","filename",MetaCommand::STRING,false);

  command.SetOption("dirfile","D",false,"Specify a file listing all the directories");
  command.SetOption("blacklist","b",false,"Specify a black list of words");
  command.AddOptionField("blacklist","filename",MetaCommand::STRING,false);
  command.SetOption("dart","dart",false,"Write out files to be send to the dart server");
  command.AddOptionField("dart","filename",MetaCommand::STRING,true);
  command.AddOptionField("dart","maxerror",MetaCommand::INT,false,"-1");
  command.AddOptionField("dart","group",MetaCommand::INT,false,"0");

  command.SetOption("kwsurl","kwsurl",false,"Specify the base url of the KWStyle HTML report");
  command.AddOptionField("kwsurl","url",MetaCommand::STRING,true);

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
  AddFeature("Spaces","0",true);
  AddFeature("Comments","/**, *, */,true",true);
  AddFeature("Header","c:/Julien/Workspace/KWStyle/kwsHeader.h,false,true",true);
  AddFeature("Indent","kws::SPACE,2,true,true",true);
  AddFeature("Namespace","itk",true);
  AddFeature("NameOfClass","[NameOfClass],itk",true);
  AddFeature("IfNDefDefine","__[NameOfClass]_[Extension]",true);
  AddFeature("EmptyLines","2",true); 
  AddFeature("Template","T",true); 
  AddFeature("Operator","1,1",true);

  if(command.GetOptionWasSet("blacklist"))
    {
    std::string blacklist = command.GetValueAsString("blacklist","filename");
    AddFeature("BlackList",blacklist.c_str(),true);
    }

  // If we should look the definition from the xml file
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

  
  // If we should look at some overwritten rules
  if(command.GetOptionWasSet("overwrite"))
    {
    std::string overwrite = command.GetValueAsString("overwrite","filename");
    
    // Read the file
    std::ifstream file;
    file.open(overwrite.c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open file: " << overwrite.c_str() << std::endl;
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
   
    long int start = 0;
    long int pos = buffer.find("\n",start);
    do    
      {
      std::string line = "";

      if(pos == -1)
        {
        line = buffer.substr(start,buffer.length()-start);
        pos = fileSize; // we stop
        }
      else
        {
        line = buffer.substr(start,pos-start);
        start = pos+1;
        }
      if(line.size() < 2)
        {
        break;
        }

      long int p = line.find(" ");
      if(p != -1)
        {
        kwsFeature f;
        f.filename = line.substr(0,p);
        long int p1 = p;
        p = line.find(" ",p+1);
        if(p!=-1)
          {
          f.name = line.substr(p1+1,p-p1-1); 
          }
        p1 = p;
        p = line.find(" ",p+1);
        std::string enablestring = line.substr(p1+1,p-p1-1);
        if(enablestring.find("Enable") != -1)
          {
          f.enable = true;
          }
        else
          {
          f.enable = false;
          }

        p1 = p;
        p = line.find("\n",p+1);
          
        if(p!=-1)
          {
          f.value = line.substr(p1+1,p-p1-1);
          }
        
        overwriteFeatures.push_back(f);
        }

      if(pos != fileSize)
        {
        pos = buffer.find("\n",start);
        }
      } while(pos<(long int)fileSize);
 
    file.close();
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
   
  std::vector<std::string> filenames;
  std::vector<kws::Parser> m_Parsers;

  // if the -d command is used
  if(parseDirectory)
    {
    bool recursive = false;
    if(command.GetOptionWasSet("recursive"))
      {
      recursive = true;
      }
    AddDirectory(inputFilename.c_str(),filenames,recursive);

    }

  // if the -D command is used
  else if(command.GetOptionWasSet("dirfile"))
    {
    // Read the file
    std::ifstream file;
    file.open(inputFilename.c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open file: " << inputFilename.c_str() << std::endl;
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
   
    long int start = 0;
    long int pos = buffer.find("\n",start);
    do    
      {
      std::string dirname = "";

      if(pos == -1)
        {
        dirname = buffer.substr(start,buffer.length()-start);
        pos = fileSize; // we stop
        }
      else
        {
        dirname = buffer.substr(start,pos-start);
        start = pos+1;
        }
      if(dirname.size() < 2)
        {
        break;
        }

      bool recursive = false;
      if(dirname.find("[R]",0) != -1)
        {
        recursive = true;
        }

      long int space = dirname.find(" ");
      if(space != -1)
        {
        dirname = dirname.substr(0,space);
        }

      // Add a / if necessary
      if((dirname[dirname.length()-1] != '/') && (dirname[dirname.length()-1] != '\\'))
        {
        dirname += "/";
        }

      AddDirectory(dirname.c_str(),filenames,recursive);

      if(pos != fileSize)
        {
        pos = buffer.find("\n",start);
        }
      } while(pos<(long int)fileSize);
 
    file.close();
    }
   else
    {
    filenames.push_back(inputFilename);
    }

  // sort the filenames
  std::sort(filenames.begin(), filenames.end());

  std::vector<std::string>::const_iterator it = filenames.begin();

  unsigned long errors =0;

  while(it != filenames.end())
    {
    if(!command.GetOptionWasSet("quiteverbose") &&
       !command.GetOptionWasSet("exporthtml")
      )
      {
      std::cout << "Processing " << (*it).c_str() << std::endl;
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
      // Check if we have a match in the list of overwriteFeatures
      bool checked = false;
      std::vector<kwsFeature>::iterator itof = overwriteFeatures.begin();
      while(itof != overwriteFeatures.end())
        {
        if(
          ((*it).find((*itof).filename.c_str()) != -1) 
           && (!strcmp((*itof).name.c_str(),(*itf).name.c_str())))
          {
          if((*itof).enable)
            {
            parser.Check((*itof).name.c_str(),(*itof).value.c_str());
            }
          checked = true;
          }
        itof++;
        }

      if(!checked && (*itf).enable)
        {
        parser.Check((*itf).name.c_str(),(*itf).value.c_str());
        }
      itf++;
      }

    // If we should display the error
    if(command.GetOptionWasSet("verbose"))
      {
      std::cout << parser.GetLastErrors().c_str() << std::endl;
      }
        
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

    bool showNoErrors = command.GetOptionWasSet("lesshtml");
    generator.GenerateHTML(html.c_str(),!showNoErrors);
    }

  // If we should export the html report
  if(command.GetOptionWasSet("exporthtml"))
    {
    kws::Generator generator;
    generator.SetParser(&m_Parsers);
    generator.ExportHTML(std::cout);
    }

  // If we should generate dart files
  if(command.GetOptionWasSet("dart"))
    {
    std::string dart = command.GetValueAsString("dart","filename");
    int maxerror = command.GetValueAsInt("dart","maxerror");
    bool grouperrors = command.GetValueAsBool("dart","group");
    kws::Generator generator;
    generator.SetParser(&m_Parsers);

    std::string url = "";
    if(command.GetOptionWasSet("kwsurl"))
      {
      url = command.GetValueAsString("kwsurl","url");
      }

    generator.GenerateDart(dart.c_str(),maxerror,grouperrors,url,time0);
    }

  return 1;
}
