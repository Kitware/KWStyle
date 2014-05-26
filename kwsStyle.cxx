/*========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsStyle.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
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
#include <kwssys/Directory.hxx>
#include <kwssys/SystemTools.hxx>
#include <kwssys/Glob.hxx>
#include <cmath>
#include <string.h>
#include <sstream>
#include "kwsXMLReader.h"
#include <vector>
#include "KWStyleConfigure.h"

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

/** Remove files */
void RemoveFile(const char* regEx,std::vector<std::string> & filenames)
{
  kwssys::RegularExpression regex(regEx);
  std::vector<std::string>::iterator it = filenames.begin(); 
  
  while(it != filenames.end())
    {
    if(regex.find((*it).c_str()))
      {
      it = filenames.erase(it);
      }
    else
      {
      it++;
      }
    }
}

int main(int argc, char **argv)
{
  double time0 = kwssys::SystemTools::GetTime();

  MetaCommand command;
  command.SetVersion(KWSTYLE_VERSION_STRING);
  
  // Check if -cvs is defined and put MetaCommand in quiet mode
  /*for(int j=0;j<argc;j++)
    {
    if(!strcmp(argv[j],"-cvs"))
      {
      command.SetVerbose(false);
      }
    }*/  

  command.SetOption("directory","d",false,"Specify a directory");
  command.SetOption("recursive","R",false,"Associated with -d recurse through directories");
  command.SetOption("verbose","v",false,"Display errors");
  command.SetOption("quiteverbose","qv",false,"Display less information");
  command.SetOption("lesshtml","lesshtml",false,"Display less HTML");

  command.SetOption("vim","vim",false,"Generate errors as VIM format");
  command.SetOption("msvc","msvc",false,"Generate errors as MSVC format");
  command.SetOption("gcc","gcc",false,"Generate errors as GCC format");

  command.SetOption("html","html",false,"Generate the HTML report");
  command.AddOptionField("html","filename",MetaCommand::STRING,false);
  command.SetOption("cvs","cvs",false,"Using KWStyle as a cvs precommit script");
  command.SetOption("exporthtml","exporthtml",false,"Export the HTML report online");
  command.SetOption("xml","xml",false,"Read a XML configure file");
  command.AddOptionField("xml","filename",MetaCommand::STRING,false);
  command.SetOption("exportxml","exportxml",false,"Write output the report as a simple XML");
  command.AddOptionField("exportxml","filename",MetaCommand::STRING,false);

  command.SetOption("overwrite","o",false,"Overwrite rules file");
  command.AddOptionField("overwrite","filename",MetaCommand::STRING,false);

  command.SetOption("dirfile","D",false,"Specify a file listing all the directories");
  command.SetOption("basedirectory","B",false,"Specify the base directory of the file");
  command.AddOptionField("basedirectory","filename",MetaCommand::STRING,false);
    
  command.SetOption("blacklist","b",false,"Specify a black list of words");
  command.AddOptionField("blacklist","filename",MetaCommand::STRING,false);
  command.SetOption("dart","dart",false,"Write out files to be send to the dart server");
  command.AddOptionField("dart","filename",MetaCommand::STRING,true);
  command.AddOptionField("dart","maxerror",MetaCommand::INT,false,"-1");
  command.AddOptionField("dart","group",MetaCommand::INT,false,"0");


  command.SetOption("kwsurl","kwsurl",false,"Specify the base url of the KWStyle HTML report");
  command.AddOptionField("kwsurl","url",MetaCommand::STRING,true);

  command.SetOption("fixFile","fix",false,"Write out a fixed version of the parsed file");

  command.AddField("infile","input filename",MetaCommand::STRING,true);

  // Parsing
  if(!command.Parse(argc,argv))
    {
    return 1;
    }

  // Add the features
  AddFeature("Header","dummyheader.h,false,true",true); // should be first
  AddFeature("LineLength","80",true);
  AddFeature("DeclarationOrder","0,1,2",true);
  AddFeature("Typedefs","[A-Z],true",true);
  AddFeature("InternalVariables","m_[A-Z],true",true);
  AddFeature("Variables","this->[A-Z]",true);
  AddFeature("Struct","[a-z]",true);
  AddFeature("SemicolonSpace","0",true);
  AddFeature("EndOfFileNewLine","",true);
  AddFeature("Tabs","",true);
  AddFeature("Spaces","3",true);
  AddFeature("Comments","/**, *, */,true,true,true",true);
  AddFeature("Indent","kws::SPACE,2,true,true",true);
  AddFeature("Namespace","itk",true);
  AddFeature("NameOfClass","[NameOfClass],itk",true);
  AddFeature("IfNDefDefine","__[NameOfClass]_[Extension]",true);
  AddFeature("EmptyLines","2",true); 
  AddFeature("Template","T",true); 
  AddFeature("Operator","1,1",true);
  AddFeature("StatementPerLine","1",true);
  AddFeature("VariablePerLine","1",true);
  AddFeature("BadCharacters","true",true);
  AddFeature("MemberFunctions","*",true);
  AddFeature("Functions","*",true);
  AddFeature("UsingDirectives", "true", true);
  AddFeature("RelativePathInInclude", "true", true);

  std::string xmlFile = "KWStyle.xml";
  // If we should look the definition from the xml file
  if(command.GetOptionWasSet("xml"))
    {
    xmlFile = command.GetValueAsString("xml","filename");
    }

  // Initialize the configuration parameters
  std::string blacklist = "";
  std::string overwrite = "";
  std::string recursive = "";
  std::string verbose = "";
  std::string htmlDirectory = "";
  std::string lessHTML = "";
  std::string filesToCheck = "";
  std::string directoryToCheck = "";
  std::string fileToCheck = "";
  std::string fixedFile = "";

  if(xmlFile.size()>0)
    {
    kws::XMLReader reader;
    if(reader.Open(xmlFile.c_str()))
      {
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

      // Read the current configuration
      blacklist = reader.GetValue("BlackList");
      overwrite = reader.GetValue("OverwriteFile");
      verbose = reader.GetValue("Verbose");
      if(verbose.size()>0 && verbose[0] != '1')
        {
        verbose = "";
        }

      recursive = reader.GetValue("Recursive");
      if(recursive.size()>0 && recursive[0] != '1')
        {
        recursive = "";
        }
      htmlDirectory = reader.GetValue("HTMLDirectory");
      lessHTML = reader.GetValue("LessHTML");
      filesToCheck = reader.GetValue("Files");
      directoryToCheck = reader.GetValue("Directory");
      fileToCheck = reader.GetValue("File");

      reader.Close();
      }
    else
      {
      std::cout << "Cannot open configuration file: " << xmlFile.c_str() << std::endl;
      }
    }
  
  if(command.GetOptionWasSet("blacklist"))
    {
    blacklist = command.GetValueAsString("blacklist","filename");
    }
  if(blacklist.size()>0)
    {
    AddFeature("BlackList",blacklist.c_str(),true);
    }

  // If we should look at some overwritten rules
  if(command.GetOptionWasSet("overwrite"))
    {
    overwrite = command.GetValueAsString("overwrite","filename");
    }

  if(overwrite.size()>0)
    {
    // Read the file
    std::ifstream file;
    file.open(overwrite.c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open overwrite file: " << overwrite.c_str() << std::endl;
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
        if(enablestring.find("Enable") != std::string::npos)
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

      if(static_cast<unsigned long>(pos) != fileSize)
        {
        pos = buffer.find("\n",start);
        }
      } while(pos<(long int)fileSize);
 
    file.close();
    }

  std::string inputFilename = command.GetValueAsString("infile");

  if(fileToCheck.size()>0)
    {
    inputFilename = fileToCheck;
    }

  if(command.GetOptionWasSet("recursive"))
    {
    recursive = "1";
    }
  // if the -D command is used
  if(command.GetOptionWasSet("dirfile"))
    {
    filesToCheck = inputFilename;
    }

  if(command.GetValueAsString("directory").length() > 0)
    {
    directoryToCheck = inputFilename;
    if(directoryToCheck[directoryToCheck.size()-1] != '/')
      {
      directoryToCheck += '/';
      }
    }
   
  std::vector<std::string> filenames;
  std::vector<kws::Parser> m_Parsers;

  // if the -d command is used
  if(directoryToCheck.size()>0)
    {
    kwssys::Glob glob;
    if(recursive.size()>0)
      {
      glob.RecurseOn();
      }
    std::string globoption = directoryToCheck.c_str();
    
    if(kwssys::SystemTools::FileExists(directoryToCheck.c_str()))
      {
      globoption += "*.*";
      }

    glob.FindFiles(globoption.c_str());
    filenames = glob.GetFiles();
    }
  else if(filesToCheck.size()>0)
    {
    // Read the file
    std::ifstream file;
    file.open(filesToCheck.c_str(), std::ios::binary | std::ios::in);
    if(!file.is_open())
      {
      std::cout << "Cannot open description files: " << filesToCheck.c_str() << std::endl;
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
    size_t pos = buffer.find("\n",start);
    size_t posr = buffer.find("\r",start);
    if(posr == std::string::npos)
      {
      posr = pos;
      }

    // Define a parser to find if we are between quotes
    kws::Parser helperParser;

    do    
      {
      std::string dirname = "";

      if(pos == std::string::npos)
        {
        dirname = buffer.substr(start,buffer.length()-start);
        pos = fileSize; // we stop
        }
      else
        {
        dirname = buffer.substr(start,posr-start);
        start = pos+1;
        }
      if(dirname.size() < 2)
        {
        break;
        }

      long int space = dirname.find(" ");
      while(space != -1 && helperParser.IsBetweenQuote(space,false,dirname))
        {
        space = dirname.find(" ",space+1);
        }

      // if we should remove the file
      if(dirname.find("-f",0) != std::string::npos)
        {
        std::string fileToRemove = dirname.substr(3,dirname.size()-3);
        RemoveFile(fileToRemove.c_str(),filenames);
        }
      else if(kwssys::SystemTools::FileExists(dirname.c_str()))
        {
        filenames.push_back(dirname);
        }
      else // we add a directory
        {
        bool gotrecurse = false;
        kwssys::Glob glob;
        if(dirname.find("[R]",0) != std::string::npos)
          {
          glob.RecurseOn();
          gotrecurse = true;
          }
      
        long int space = dirname.find(" ");
        while(space != -1 && helperParser.IsBetweenQuote(space,false,dirname))
          {
          space = dirname.find(" ",space+1);
          }

        if(space != -1)
          {
          dirname = dirname.substr(0,space);
          }
     
        // Remove quotes if any
        if(dirname.size()>0 && dirname[0] == '"')
          {
          dirname = dirname.substr(1,dirname.size()-2);
          }

        std::string globoption = dirname.c_str();
        glob.FindFiles(globoption.c_str());
        std::vector<std::string> globfiles = glob.GetFiles();
        std::vector<std::string>::const_iterator itglob = globfiles.begin();
        while(itglob != globfiles.end())
          {
          filenames.push_back(*itglob);
          itglob++;
          }
        }

      if(pos != fileSize)
        {
        pos = buffer.find("\n",start);
        posr = buffer.find("\r",start);
        if(posr == std::string::npos)
          {
          posr = pos;
          }
        }
      } while(pos<fileSize);
 
    file.close();
    }
   else
    {
    filenames.push_back(inputFilename);
    }

  // sort the filenames
  std::sort(filenames.begin(), filenames.end());

  
  // if the -cvs command is used
  // WARNING this option should be last because its position is used
  // to determine the filenames.
  /*if(command.GetOptionWasSet("cvs"))
    {
    filenames.clear();
    int cvspos = 0;
    int i=0;
    // Look for the position of the cvs command
    for(i=0;i<argc;i++)
      {
      if(!strcmp(argv[i],"-cvs"))
        {
        cvspos = i;
        break;
        }
      }     
    cvspos++;
    std::string cvsdir = argv[cvspos];
    cvspos++;
    for(i=cvspos;i<argc;i++)
      {
      std::string filename = argv[i];
      if((filename.substr(filename.size()-4,4) == ".hxx")
        || (filename.substr(filename.size()-4,4) == ".txx")
        || (filename.substr(filename.size()-4,4) == ".cxx")
        || (filename.substr(filename.size()-2,2) == ".h")
        )
        {
        filenames.push_back(filename);
        }
      }
    }*/

  std::vector<std::string>::const_iterator it = filenames.begin();

  unsigned long nerrors =0;

  while(it != filenames.end())
    {
    if(!command.GetOptionWasSet("quiteverbose") &&
       !command.GetOptionWasSet("exporthtml") &&
       !command.GetOptionWasSet("vim") && !command.GetOptionWasSet("msvc")
       && !command.GetOptionWasSet("gcc")
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
      // When removing a file with CVS we should return 0 otherwise it fails
      if(command.GetOptionWasSet("cvs"))
        {
        return 0;
        }
      return 1;
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

    // Ask the parser to generate a corrected version of the file
    if(command.GetOptionWasSet("fixFile"))
      {
      parser.SetFixFile(true);
      }
    
    std::vector<kwsFeature>::iterator itf = features.begin();
    while(itf != features.end())
      {
      // Check if we have a match in the list of overwriteFeatures
      bool checked = false;
      std::vector<kwsFeature>::iterator itof = overwriteFeatures.begin();
      while(itof != overwriteFeatures.end())
        {        
        // Allow for for regex expression within overwrite files
        kwssys::RegularExpression regex((*itof).filename.c_str());

        if( regex.find((*it).c_str())
           && (!strcmp((*itof).name.c_str(),(*itf).name.c_str())))
          { 
          if((*itof).enable)
            {
            parser.Check((*itof).name.c_str(),(*itof).value.c_str());
            }

          if((*itof).name == "LineLength" && !((*itof).enable))
            {
            std::cout << "Cannot disable LineLength check. It computes internal parameters" << std::endl;
            }  
          else
            {  
            checked = true;
            }
          }
        itof++;
        }

      if(!checked && (*itf).enable)
        {
        parser.Check((*itf).name.c_str(),(*itf).value.c_str());
        }
      itf++;
      }

   if(command.GetOptionWasSet("verbose"))
     {
     verbose = "1";
     }

    // If we should display the error
    if(verbose.size()>0 
      && !command.GetOptionWasSet("msvc")
      && !command.GetOptionWasSet("vim")
      && !command.GetOptionWasSet("gcc")
      )
      {
      std::cout << parser.GetLastErrors().c_str() << std::endl;
      std::cout << parser.GetLastWarnings().c_str() << std::endl;
      }

    if(command.GetOptionWasSet("vim"))
      {
      // Format the string for vim
      const kws::Parser::ErrorVectorType errors = parser.GetErrors();
      kws::Parser::ErrorVectorType::const_iterator eit = errors.begin();
      while(eit != errors.end())
        {
        std::cout << (*it).c_str() << ":" << (*eit).line << ":" 
                  << (*eit).description << std::endl;
        eit++;
        }
      }
    else if(command.GetOptionWasSet("msvc"))
      {
      // Format the string for Visual Studio
      const kws::Parser::ErrorVectorType errors = parser.GetErrors();
      kws::Parser::ErrorVectorType::const_iterator eit = errors.begin();
      while(eit != errors.end())
        {
        std::cout << (*it).c_str() << "(" << (*eit).line << ") : error " << (*eit).number << ":"  
                  << (*eit).description << std::endl;
        eit++;
        }
      const kws::Parser::WarningVectorType warnings = parser.GetWarnings();
      kws::Parser::WarningVectorType::const_iterator wit = warnings.begin();
      while(wit != warnings.end())
        {
        std::cout << (*it).c_str() << "(" << (*wit).line << ") : warning " << (*wit).number << ":"  
                  << (*wit).description << std::endl;
        wit++;
        }
      }
     else if(command.GetOptionWasSet("gcc"))
      {
      // Format the string for Visual Studio
      const kws::Parser::ErrorVectorType errors = parser.GetErrors();
      kws::Parser::ErrorVectorType::const_iterator eit = errors.begin();
      while(eit != errors.end())
        {
        std::cout << (*it).c_str() << ":" << (*eit).line << ": error: " 
                  << (*eit).description << std::endl;
        eit++;
        }
      const kws::Parser::WarningVectorType warnings = parser.GetWarnings();
      kws::Parser::WarningVectorType::const_iterator wit = warnings.begin();
      while(wit != warnings.end())
        {
        std::cout << (*it).c_str() << ":" << (*wit).line << ": warning: " 
                  << (*wit).description << std::endl;
        wit++;
        }
      }
            
    if(command.GetOptionWasSet("cvs"))
      {
      std::cout << parser.GetLastErrors().c_str() << std::endl;
      if(parser.GetErrors().size()>0)
        {
        return 1;
        }
      }
    
    nerrors += parser.GetErrors().size();

    parser.GenerateFixedFile();

    m_Parsers.push_back(parser);
    it++;
    } // end filenames

  if(command.GetOptionWasSet("html"))
    {
    htmlDirectory = command.GetValueAsString("html","filename");
    }

  // If we should generate the HTML file
  if(htmlDirectory.size()>0)
    {
    kws::Generator generator;
    generator.SetParser(&m_Parsers);

    if(command.GetOptionWasSet("xml"))
      {
      std::string xml = command.GetValueAsString("xml","filename");
      generator.ReadConfigurationFile(xml.c_str());
      }

    bool showNoErrors = false;
    if(lessHTML.size()>0)
      {
      if(lessHTML[0] == '1')
        {
        showNoErrors = true;
        }
      }
    
    if(command.GetOptionWasSet("lesshtml"))
      {
      showNoErrors = true;
      }
    generator.GenerateHTML(htmlDirectory.c_str(),!showNoErrors);
    }

  // If we should export the html report
  if(command.GetOptionWasSet("exporthtml"))
    {
    kws::Generator generator;
    if(command.GetOptionWasSet("xml"))
      {
      std::string xml = command.GetValueAsString("xml","filename");
      generator.ReadConfigurationFile(xml.c_str());
      }
    generator.SetParser(&m_Parsers);
    generator.ExportHTML(std::cout);
    }

  if(command.GetOptionWasSet("exportxml"))
    {
    std::string outputxmlFile = command.GetValueAsString("exportxml","filename");
    kws::Generator generator;
    generator.SetParser(&m_Parsers);
    generator.ExportXML(outputxmlFile.c_str());
    }

  // If we should generate dart files
  if(command.GetOptionWasSet("dart"))
    {
    std::string dart = command.GetValueAsString("dart","filename");
    int maxerror = command.GetValueAsInt("dart","maxerror");
    bool grouperrors = command.GetValueAsBool("dart","group");
    kws::Generator generator;
    if(command.GetOptionWasSet("xml"))
      {
      std::string xml = command.GetValueAsString("xml","filename");
      generator.ReadConfigurationFile(xml.c_str());
      }
    generator.SetParser(&m_Parsers);

    std::string url = "";
    if(command.GetOptionWasSet("kwsurl"))
      {
      url = command.GetValueAsString("kwsurl","url");
      }
    
    std::string baseDirectory = "";
    
    if(command.GetOptionWasSet("basedirectory"))
      {
      baseDirectory = command.GetValueAsString("basedirectory","filename");
      }
    generator.GenerateDart(dart.c_str(),maxerror,grouperrors,url,time0,baseDirectory.c_str());
    }

  // If we have errors we return false
  if(nerrors>0)
    {
    return 1;
    }

  return 0;
}
