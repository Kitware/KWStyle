/*=========================================================================

  Program:   KWStyle - Kitware Style Checker
  Module:    kwsGenerator.cxx

  Copyright (c) Kitware, Inc.  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "kwsGenerator.h"
#include "kwsXMLReader.h"

namespace kws {

/** Constructor */
Generator::Generator()
{
  m_Parsers = NULL;
  m_ProjectTitle = "";
  m_ProjectLogo = "Logo.gif";
  m_KWStyleLogo = "kwstylelogo.jpg";
  m_MaxDirectoryDepth = 99;
  m_ErrorThreshold = 0;
}

/** Destructor */
Generator::~Generator()
{
}

/** Read the configuration file */
void Generator::ReadConfigurationFile(const char* configFile) 
{
  kws::XMLReader reader;
  if(reader.Open(configFile))
    {
    m_ProjectTitle = reader.GetValue("Project");
    m_ProjectLogo = reader.GetValue("ProjectLogo");
    m_KWStyleLogo = reader.GetValue("KWStyleLogo");
    m_ErrorThreshold = atoi(reader.GetValue("ErrorThreshold").c_str());
    reader.Close();
    }
}

/** Generate the description of the test.
 *  For the moment we assume that all the test have the same description */
bool Generator::GenerateDescription(const char* dir)
{
  std::cout << "Generating Description...";

  std::string filename = dir;
  if((dir[strlen(dir)-1] != '/') && dir[strlen(dir)-1] != '\\')
    {
    filename += "/";
    }
  filename += "KWSDescription.html";

  std::ofstream file;
  file.open(filename.c_str(), std::ios::binary | std::ios::out);
  if(!file.is_open())
    {
    std::cout << "Cannot open file for writing: " <<  std::endl;
    return false;
    }
  
  std::string title = filename;
  if(m_ProjectTitle.size() > 0)
    {
    title = "Description for " + m_ProjectTitle;
    }
  this->CreateHeader(&file,title.c_str());
  
  file << "<br>" << std::endl;

  if(m_Parsers->size() > 0)
    {
    ParserVectorType::const_iterator it = m_Parsers->begin();
    for(unsigned int i=0;i<NUMBER_ERRORS;i++)
      {
      if((*it).GetTestDescription(i) != "NA")
        {
        file << "<font color=\"" << ErrorColor[i] << "\">" << std::endl;
        file << ErrorTag[i]  << std::endl;
        file << "</font> : " << std::endl;
        file << (*it).GetTestDescription(i).c_str() << std::endl;
        file << "<br>" << std::endl;
        }
      }
    }

  this->CreateFooter(&file);    
  file.close();

  std::cout << "done" << std::endl;
  return true;
}

/** Generate the Matrix representation */
bool Generator::GenerateMatrix(const char* dir,bool showAllErrors)
{
  std::cout << "Generating Matrix...";

  std::string filename = dir;
  if((dir[strlen(dir)-1] != '/') && dir[strlen(dir)-1] != '\\')
    {
    filename += "/";
    }
  filename += "KWSMatrix.html";

  std::ofstream file;
  file.open(filename.c_str(), std::ios::binary | std::ios::out);
  if(!file.is_open())
    {
    std::cout << "Cannot open file for writing: " <<  std::endl;
    return false;
    }
  
  std::string title = filename;
  if(m_ProjectTitle.size() > 0)
    {
    title = "Matrix for " + m_ProjectTitle;
    }
  this->CreateHeader(&file,title.c_str());

  // Contruct the table
  file << "<table width=\"100%\" border=\"0\" height=\"1\">" << std::endl;
  file << "<tr>" << std::endl;
  file << "  <td width=\"30%\"> " << std::endl;
  file << "    <div align=\"center\">Filename</div>" << std::endl;
  file << "  </td>" << std::endl;

  // Check the number of tests that have been performed
  bool tests[NUMBER_ERRORS];
  for(unsigned int i=0;i<NUMBER_ERRORS;i++)
    {
    tests[i] = false;
    }

  ParserVectorType::const_iterator it = m_Parsers->begin();
  while(it != m_Parsers->end())
    {
    for(unsigned int i=0;i<NUMBER_ERRORS;i++)
      {
      if((*it).HasBeenPerformed(i))
        {
        tests[i] = true;
        }
      }
    it++;
    }
  unsigned int nTests = 0;
  for(unsigned int i=0;i<NUMBER_ERRORS;i++)
    {
    if(tests[i])
      {
      nTests++;
      }
    }

  unsigned int width = 70/nTests;
  for(unsigned int i=0;i<NUMBER_ERRORS;i++)
    {
    if(!tests[i])
      {
      continue;
      }
    file << "  <td width=\"" << width << "%\">" << std::endl;
    file << "    <div align=\"center\">" << ErrorTag[i] << "</div>" << std::endl;
    file << "  </td>" << std::endl;
    }
  file << "</tr>" << std::endl;

  it = m_Parsers->begin();
  std::string currentPath = "";
  while(it != m_Parsers->end())
    {
    
    if(!showAllErrors && (*it).GetErrors().size() == 0)
      {
      it++;
      continue;
      }

    // If we have a new directory we show it on a new line
    std::string filenamePath = kwssys::SystemTools::GetFilenamePath((*it).GetFilename());
    
    if(currentPath != filenamePath)
      {
      file << "<tr>" << std::endl;
      file << "  <td width=\"30%\"></td>" << std::endl;
      file << "  <td bgcolor=\"#CCCCCC\" width=\"70%\" colspan=\"" << nTests << "\"><div align=\"center\">" << std::endl;;
      file << filenamePath.c_str() << std::endl;
      file << "</div></td></tr>" << std::endl;
      currentPath = filenamePath;
      }

    // Replace '/' by '_' (and strip any colon)
    std::string filename = (*it).GetFilename();
    if(long int pos = filename.find(":/") != -1)
      {
      filename = filename.substr(pos+2,filename.size()-pos-2);
      }
    if(long int pos = filename.find(":\\") != -1)
      {
      filename = filename.substr(pos+2,filename.size()-pos-2);
      }

    long int slash = filename.find_last_of("/");
    unsigned int i=0;
    while(slash != -1 && i<m_MaxDirectoryDepth) 
      {
      filename.replace(slash,1,"_");
      slash = filename.find_last_of("/");
      i++;
      }
    slash = filename.find_last_of("/");
    if(slash != -1)
      {
      filename = filename.substr(slash+1,filename.size()-slash-1);
      }

    slash = (*it).GetFilename().find_last_of("/");

    std::string nameofclass = (*it).GetFilename().substr(slash+1,((*it).GetFilename().size())-slash-1);
    std::string filenamecorrect = nameofclass;
    filename += ".html";

    // Fill in the table
    file << "<tr>" << std::endl;
    file << "  <td width=\"30%\"> " << std::endl;
    file << "    <div align=\"center\"> <a href=\"" << filename.c_str()  << "\">" << filenamecorrect.c_str() << "</a></div>" << std::endl;
    file << "  </td>" << std::endl;

    unsigned int width = 50/nTests;
    for(unsigned int i=0;i<NUMBER_ERRORS;i++)
      {
      if(!tests[i])
        {
        continue;
        }
      // Count the number of errors for this type of error
      unsigned int nerror = 0;
      
      Parser::ErrorVectorType errors = (*it).GetErrors();
      Parser::ErrorVectorType::const_iterator itError = errors.begin();
      while(itError != errors.end())
        {
        if((*itError).number == i)
          {
          nerror++;
          }
        itError++;
        }

      file << "  <td width=\"" << width << "%\"";
      if(nerror == 0)
        {
        if(!(*it).HasBeenPerformed(i))
          {
          file << "bgcolor=\"#cccccc\"";
          }
        else
          {
          file << "bgcolor=\"#00aa00\"";
          }
        }
      else if(nerror <= m_ErrorThreshold)
        {
        file << "bgcolor=\"#ffcc66\"";
        }
      else
        {
        file << "bgcolor=\"#ff6666\"";
        }
      file << ">" << std::endl;
      file << "    <div align=\"center\">" << nerror << "</div>" << std::endl;
      file << "  </td>" << std::endl;
      }
    file << "</tr>" << std::endl; 

    it++;
    }

  file << "</table>" << std::endl;
  
  this->CreateFooter(&file);    
  file.close();

  std::cout << "done" << std::endl;
  return true;
}

/** Generate the HTML files */
bool Generator::GenerateHTML(const char* dir,bool showAllErrors)
{
  // Check if the directory exists. If not create it
  kwssys::Directory directory;
  if(!directory.Load(dir))
    {
    std::cout << "Creating HTML directory: " << dir << std::endl;
    if(!kwssys::SystemTools::MakeDirectory(dir))
      {
      std::cout << "Cannot create HTML directory: " << dir << std::endl;
      }
    }

  std::string imagedir = dir;
  imagedir += "/images";

  // Copy the m_ProjectLogo and m_KWStyleLogo to the images directory
  if(m_ProjectLogo.size()>0 || m_KWStyleLogo.size()>0)
    {
    if(!kwssys::SystemTools::MakeDirectory(imagedir.c_str()))
      {
      std::cout << "Cannot create images directory: " 
                << imagedir.c_str() << std::endl;
      }
    }

  if(m_ProjectLogo.size()>0)
    {
    std::string imageProjectLogo = imagedir;
    imageProjectLogo += "/";
    imageProjectLogo += kwssys::SystemTools::GetFilenameName(m_ProjectLogo.c_str());
    if(!kwssys::SystemTools::CopyFileIfDifferent(m_ProjectLogo.c_str(),imageProjectLogo.c_str()))
      {
      std::cout << "Cannot copy the project logo file: " 
                << m_ProjectLogo.c_str() << " into " 
                << imageProjectLogo.c_str() << std::endl;
      }
    }

  if(m_KWStyleLogo.size()>0)
    {
    std::string imageKWStyleLogo = imagedir;
    imageKWStyleLogo += "/";
    imageKWStyleLogo += kwssys::SystemTools::GetFilenameName(m_KWStyleLogo.c_str());
    if(!kwssys::SystemTools::CopyFileIfDifferent(m_KWStyleLogo.c_str(),imageKWStyleLogo.c_str()))
      {
      std::cout << "Cannot copy the KWStyle logo file: " 
                << m_KWStyleLogo.c_str() << " into " 
                << imageKWStyleLogo.c_str() << std::endl;
      }
    }
 
  // Generate the matrix representation
  this->GenerateMatrix(dir,showAllErrors);
  this->GenerateDescription(dir);

  std::cout << "Generating HTML...";

  // For each file we generate an html page
  ParserVectorType::const_iterator it = m_Parsers->begin();
  while(it != m_Parsers->end())
    {
    if(!showAllErrors && (*it).GetErrors().size() == 0)
      {
      it++;
      continue;
      }

    std::ofstream file;

    if((*it).GetFilename().size() == 0)
      {
      it++;
      continue;
      }

    // Extract the filename
    // Replace '/' by '_'
    std::string filename = dir;
    filename += "/";
    std::string filename2 = (*it).GetFilename();
    if(long int pos = filename2.find(":/") != -1)
      {
      filename2 = filename2.substr(pos+2,filename2.size()-pos-2);
      }
    if(long int pos = filename2.find(":\\") != -1)
      {
      filename2 = filename2.substr(pos+2,filename2.size()-pos-2);
      }

    long int slash = filename2.find_last_of("/");
    unsigned int i = 0;
    while(slash != -1 && i<m_MaxDirectoryDepth) 
      {
      filename2.replace(slash,1,"_");
      slash = filename2.find_last_of("/");
      i++;
      }
    slash = filename2.find_last_of("/");
    if(slash != -1)
      {
      filename2 = filename2.substr(slash+1,filename2.size()-slash-1);
      }
    filename += filename2;
    
    /*
    long int slash = (*it).GetFilename().find_last_of("/");
    if(slash == -1)
      {
      slash = 0;
      }
    std::string nameofclass = (*it).GetFilename().substr(slash+1,((*it).GetFilename().size())-slash-1);  
    filename += nameofclass;
    */
    
    filename += ".html";

    file.open(filename.c_str(), std::ios::binary | std::ios::out);
    if(!file.is_open())
      {
      std::cout << "Cannot open file for writing: " <<  std::endl;
      return false;
      }

    this->CreateHeader(&file,filename.c_str());

    file << "<table width=\"100%\" border=\"0\" height=\"1\">" << std::endl;
  
    bool comment = false;
    for(unsigned int i=0;i<(*it).GetNumberOfLines();i++)
      {
      // Look in the errors if there is a match for this line
      int error = -1;
      std::string errorTag = "";

      const Parser::ErrorVectorType errors = (*it).GetErrors();
      Parser::ErrorVectorType::const_iterator itError = errors.begin();
      while(itError != errors.end())
        {
        if( ((i+1>=(*itError).line) && (i+1<=(*itError).line2))
          )
          {
          if(errorTag.size() == 0)
            {
            errorTag += (*it).GetErrorTag((*itError).number);
            }
          else
            {
            errorTag += ",";
            errorTag += (*it).GetErrorTag((*itError).number);
            }
          error = (*itError).number;
          }
        itError++;
        }

      if(error>=0)
        {
        file << "<tr bgcolor=\"" << ErrorColor[error]  << "\">" << std::endl; 
        }
      else
        {
        file << "<tr>" << std::endl;
        }
      
      // First column is the line number
      file << "<td height=\"1\">" << i+1 << "</td>" << std::endl;
      
      // Second column is the error tag
      file << "<td height=\"1\">" << errorTag.c_str() << "</td>" << std::endl;
      
      std::string l = (*it).GetLine(i);

      // If the error is of type INDENT we show the problem as *
      if(errorTag.find("IND") != -1)
        {
        unsigned int k = 0;
        while((l[k] == ' ') || (l[k] == '\n'))
          {
          if(l[k] == ' ')
            {
            l[k]='*'; 
            }
          k++;
          }
        }

      // If the error is of type extra spaces we show the problem as *
      if(errorTag.find("ESP") != -1)
        {
        int k = l.size()-1;
        while(k>0 && (l[k] == ' '))
          {
          l[k]='*'; 
          k--;
          }
        }

      // Remove the first \n
      long int p = l.find('\n');
      if(p != -1)
        {
        l.replace(p,1,"");
        }
      
      // Replace < and >
      long int inf = l.find("<",0);
      while(inf != -1)
        {
        l.replace(inf,1,"&lt;");
        inf = l.find("<",0);
        }

      long int sup = l.find(">",0);
      while(sup != -1)
        {
        l.replace(sup,1,"&gt;");
        sup = l.find(">",0);
        }

      // Replace the space by &nbsp;
      long int space = l.find(' ',0);
      while(space != -1)  
        {
        l.replace(space,1,"&nbsp;");
        space = l.find(' ',space+1);
        }

      // Show the comments in green
      if(comment)
        {
        l.insert(0,"<font color=\"#009933\">");
        }

      space = l.find("/*",0);
      while(space != -1)  
        {
        comment = true;
        l.insert(space,"<font color=\"#009933\">");
        space = l.find("/*",space+23);
        }

      if(comment)
        {
        l.insert(l.size(),"</font>");
        }

      space = l.find("*/",0);
      while(space != -1)
        {
        comment = false;
        l.insert(space+2,"</font>");
        space = l.find("*/",space+8);
        }

      // Show the comments in green
      space = l.find("//",0);
      if(space != -1)  
        {
        l.insert(space,"<font color=\"#009933\">");
        l += "<font>";
        }

      file << "<td height=\"1\"><font face=\"Courier New, Courier, mono\" size=\"2\">" << l.c_str() << "</font></td>" << std::endl;
      file << "</tr>" << std::endl;
      }

    file << "</table>" << std::endl;

    this->CreateFooter(&file);
    
    file.close();
    it++;
    }

  std::cout << "done" << std::endl;
  return true;
}

/** Create Header */
bool Generator::CreateHeader(std::ostream * file,const char* title)
{
  *file << "<html>" << std::endl;
  *file << "<HEAD>" << std::endl;
  *file << "<META name=\"description\" content=\"kitware.com\">" << std::endl;
  *file << "<META name=\"keywords\" content=\"kwstyle,Kitware,Style,Checker,Dart\">" << std::endl;
  *file << "<META name=\"author\" content=\"Kitware\">" << std::endl;
  *file << "<META name=\"revisit-after\" content=\"2 days\">" << std::endl;
  *file << "<META name=\"robots\" content=\"all\">" << std::endl;
  *file << "<title>KWStyle - " << title << "</title>" << std::endl;
  *file << "</HEAD>" << std::endl;

  // Now create the top frame
 *file << "<table width=\"100%\" border=\"0\">" << std::endl;
 *file << " <tr>" << std::endl;
 *file << "   <td width=\"15%\" height=\"2\"><img src=\"images/" << kwssys::SystemTools::GetFilenameName(m_ProjectLogo.c_str()) << "\"></td>" << std::endl;
 *file << "   <td width=\"85%\" height=\"2\" bgcolor=\"#0099CC\"> " << std::endl;

 // remove the last extension
 std::string tit = title;
 long int pos = tit.find_last_of(".");
 if(pos!=-1)
   {
   tit = tit.substr(0,pos);
   }
 pos = tit.find_last_of("/");
 if(pos!=-1)
   {
   tit = tit.substr(pos+1,tit.size()-pos-1);
   }
 
 *file << "     <div align=\"left\"><b><font color=\"#FFFFFF\" size=\"5\">KWStyle - " << tit.c_str() << "</font></b></div>" << std::endl;
 *file << "   </td>" << std::endl;
 *file << " </tr>" << std::endl;
 *file << "</table>" << std::endl;
 *file << "<table width=\"100%\" border=\"0\">" << std::endl;
 *file << " <tr> " << std::endl;
 *file << "   <td width=\"15%\" height=\"30\" >&nbsp;</td>" << std::endl;
 *file << "   <td height=\"30\" width=\"12%\" bgcolor=\"#0099CC\"> " << std::endl;
 *file << "     <div align=\"center\"><a href=\"KWSMatrix.html\">Matrix View</a></div>" << std::endl;
 *file << "   </td>" << std::endl;
 *file << "   <td width=\"10%\" height=\"30\" bgcolor=\"#0099CC\" >"<< std::endl;
 *file << " <div align=\"center\"><a href=\"KWSDescription.html\">Description</a></div>" << std::endl;
 *file << "  </td>" << std::endl;
 *file << "   <td width=\"63%\" height=\"30\"> " << std::endl;
 *file << "     <div align=\"left\"><b></b></div>" << std::endl;
 *file << "     <div align=\"right\"></div>" << std::endl;
 *file << "   </td>" << std::endl;
 *file << " </tr>" << std::endl;
 *file << "</table>" << std::endl;
 *file << "<hr size=\"1\">";
  return true;
}

/** Create Footer */
bool Generator::CreateFooter(std::ostream * file)
{
  *file << "<hr size=\"1\">";
  *file << "<table width=\"100%\" border=\"0\">";
  *file << "<tr>";
  *file << "<td>Generated by <a href=\"http://public.kitware.com/KWStyle\">KWStyle</a> 1.0b on <i>" << kwssys::SystemTools::GetCurrentDateTime("%A %B,%d at %I:%M:%S%p") << "</i></td>";
  *file << "<td>";
  if(m_KWStyleLogo.size()>0)
    {
    *file << "<div align=\"center\"><img src=\"images/" << kwssys::SystemTools::GetFilenameName(m_KWStyleLogo.c_str()) 
          << "\" height=\"49\"></div>" << std::endl;
    }
  *file << "</td>";
  *file << "<td>";
  *file << "<div align=\"right\"><a href=\"http://www.kitware.com\">&copy; Kitware Inc.</a></div></td>";
  *file << "</tr>";
  *file << "</table>";
  *file << "<br>";

  *file << "</html>" << std::endl;

  return true;
}

/** Export the HTML report */
void Generator::ExportHTML(std::ostream & output)
{
  // For each file we generate an html page
  ParserVectorType::const_iterator it = m_Parsers->begin();
  while(it != m_Parsers->end())
    {
    if((*it).GetFilename().size() == 0)
      {
      it++;
      continue;
      }

    // Extract the filename
    std::string filename = "";
    long int slash = (*it).GetFilename().find_last_of("/");
    if(slash == -1)
      {
      slash = 0;
      }
    std::string nameofclass = (*it).GetFilename().substr(slash+1,((*it).GetFilename().size())-slash-1);  
    filename += nameofclass;
    filename += ".html";

    //this->CreateHeader(&output,filename.c_str());

    output << "<table width=\"100%\" border=\"0\" height=\"1\">" << std::endl;
  
    bool comment = false;
    for(unsigned int i=0;i<(*it).GetNumberOfLines();i++)
      {
      // Look in the errors if there is a match for this line
      int error = -1;
      std::string errorTag = "";

      const Parser::ErrorVectorType errors = (*it).GetErrors();
      Parser::ErrorVectorType::const_iterator itError = errors.begin();
      while(itError != errors.end())
        {
        if( ((i+1>=(*itError).line) && (i+1<=(*itError).line2))
          )
          {
          if(errorTag.size() == 0)
            {
            errorTag += (*it).GetErrorTag((*itError).number);
            }
          else
            {
            errorTag += ",";
            errorTag += (*it).GetErrorTag((*itError).number);
            }
          error = (*itError).number;
          }
        itError++;
        }

      if(error>=0)
        {
        output << "<tr bgcolor=\"" << ErrorColor[error]  << "\">" << std::endl; 
        }
      else
        {
        output << "<tr>" << std::endl;
        }
      
      // First column is the line number
      output << "<td height=\"1\">" << i+1 << "</td>" << std::endl;
      
      // Second column is the error tag
      output << "<td height=\"1\">" << errorTag.c_str() << "</td>" << std::endl;
      
      std::string l = (*it).GetLine(i);

      // If the error is of type INDENT we show the problem as _
      if(errorTag.find("IND") != -1)
        {
        unsigned int k = 0;
        while((l[k] == ' ') || (l[k] == '\n'))
          {
          if(l[k] == ' ')
            {
            l[k]='*'; 
            }
          k++;
          }
        }

      // Remove the first \n
      long int p = l.find('\n');
      if(p != -1)
        {
        l.replace(p,1,"");
        }
      
      // Replace < and >
      long int inf = l.find("<",0);
      while(inf != -1)
        {
        l.replace(inf,1,"&lt;");
        inf = l.find("<",0);
        }

      long int sup = l.find(">",0);
      while(sup != -1)
        {
        l.replace(sup,1,"&gt;");
        sup = l.find(">",0);
        }

      // Replace the space by &nbsp;
      long int space = l.find(' ',0);
      while(space != -1)  
        {
        l.replace(space,1,"&nbsp;");
        space = l.find(' ',space+1);
        }

      // Show the comments in green
      if(comment)
        {
        l.insert(0,"<font color=\"#009933\">");
        }

      space = l.find("/*",0);
      while(space != -1)  
        {
        comment = true;
        l.insert(space,"<font color=\"#009933\">");
        space = l.find("/*",space+23);
        }

      if(comment)
        {
        l.insert(l.size(),"</font>");
        }

      space = l.find("*/",0);
      while(space != -1)
        {
        comment = false;
        l.insert(space+2,"</font>");
        space = l.find("*/",space+8);
        }

      // Show the comments in green
      space = l.find("//",0);
      if(space != -1)  
        {
        l.insert(space,"<font color=\"#009933\">");
        l += "<font>";
        }

      output << "<td height=\"1\"><font face=\"Courier New, Courier, mono\" size=\"2\">" << l.c_str() << "</font></td>" << std::endl;
      output << "</tr>" << std::endl;
      }

    output << "</table>" << std::endl;

    this->CreateFooter(&output);
    it++;
    }
}

/** Generate dart files */
bool Generator::GenerateDart(const char* dir,int maxError,bool group,std::string url,double time)
{
  std::cout << "Generating Dart...";
 
  // We try to find the TAG file 
  std::string dirname = dir;
  if(dir[strlen(dir)-1] != '/' && dir[strlen(dir)-1] != '\\')
    {
     dirname += "/";
    }
  dirname += "Testing/";

  std::string tag = dirname+"TAG";
  
  std::ifstream tagfile;
  tagfile.open(tag.c_str(), std::ios::binary | std::ios::in);
  if(!tagfile.is_open())
    {
    std::cout << "Cannot open file for reading: " << tag.c_str() << std::endl;
    return false;
    }

  // We read the first line of the file which gives the path to the actual
  // directory
  char* bufferTag = new char[255];
  tagfile.getline(bufferTag,255);

  if(bufferTag[strlen(bufferTag)-1] == '\r')
    {
    bufferTag[strlen(bufferTag)-1] = '\0';
    }

  dirname += bufferTag;
  dirname += "/";

  delete [] bufferTag; 

  // We should have a Configure.xml file in the directory
  std::string configname = dirname+"Configure.xml";

  std::ifstream configfile;

  configfile.open(configname.c_str(), std::ios::binary | std::ios::in);
  if(!configfile.is_open())
    {
    std::cout << "Cannot open file for reading: " << configname.c_str() << std::endl;
    return false;
    }
  
  // Create the Build.xml file
  std::ofstream file;
  std::string filename = dirname+"Build.xml";

  file.open(filename.c_str(), std::ios::binary | std::ios::out);
  if(!file.is_open())
    {
    std::cout << "Cannot open file for writing: " <<  std::endl;
    return false;
    }

  // Generate the header
  //file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  //file << "<Site BuildName=\"Linux-gcc-3.3\" BuildStamp=\"20060207-0100-Nightly\" Name=\"voltaire.caddlab.unc\" Generator=\"ctest2.3-20050829\">" << std::endl;
  char* buffer = new char[255];
  configfile.getline(buffer,255);
  file << buffer << std::endl;
  configfile.getline(buffer,255);
  file << buffer << std::endl;

  delete [] buffer;

  file << "<Build>" << std::endl;
  file << "      <StartDateTime>";
  file << kwssys::SystemTools::GetCurrentDateTime("%b %d %I:%M:%S %z");
  file << "</StartDateTime>" << std::endl;
  file << "<BuildCommand>KWStyle</BuildCommand>" << std::endl;

  int nErrors = 0;
  ParserVectorType::const_iterator it = m_Parsers->begin();
  while(it != m_Parsers->end())
    {
    bool first = true;
    const Parser::ErrorVectorType errors = (*it).GetErrors();
    Parser::ErrorVectorType::const_iterator itError = errors.begin();
    while(itError != errors.end())
      {
      if((!group) || (first && group))
        {
        file << "<Error>" << std::endl;
        file << "          <BuildLogLine>1</BuildLogLine>" << std::endl;
        file << "          <SourceFile>";
        file << (*it).GetFilename();
        file << "</SourceFile>" << std::endl;
        file << "          <SourceLineNumber>";
        file << (*itError).line;
        file << "</SourceLineNumber>" << std::endl;

        if(url != "")
          {
          // We had a link to the dashboard
          /*long int posslash = (*it).GetFilename().find_last_of("/");
          long int posbackslash = (*it).GetFilename().find_last_of("\\");
          
          long int pos = 0;
          if(posslash != -1 && posslash>posbackslash)
            {
            pos = posslash;  
            }
          
          if(posbackslash != -1 && posbackslash>posslash)
            {
            pos = posbackslash;  
            }

          if(pos == 0) 
            {
            pos++;
            }
          std::string htmlfile = (*it).GetFilename().substr(pos+1,(*it).GetFilename().size()-pos-1);
          */

          std::string htmlfile = (*it).GetFilename();
          if(long int pos = htmlfile.find(":/") != -1)
            {
            htmlfile = htmlfile.substr(pos+2,htmlfile.size()-pos-2);
            }
          if(long int pos = htmlfile.find(":\\") != -1)
            {
            htmlfile = htmlfile.substr(pos+2,htmlfile.size()-pos-2);
            }

          long int slash = htmlfile.find_last_of("/");
          unsigned int i = 0;
          while(slash != -1 && i<m_MaxDirectoryDepth) 
            {
            htmlfile.replace(slash,1,"_");
            slash = htmlfile.find_last_of("/");
            i++;
            }
          slash = htmlfile.find_last_of("/");
          if(slash != -1)
            {
            htmlfile = htmlfile.substr(slash+1,htmlfile.size()-slash-1);
            }
          
          htmlfile += ".html";
          file << "<Url>"<< url.c_str() << "/" << htmlfile.c_str() << "</Url>" << std::endl;
          file << "<UrlName>View KWStyle File</UrlName>" << std::endl;
          }
        first = false;
        file << "          <Text>";
        }
      
      file << (*it).GetErrorTag((*itError).number);
      file << " : ";
      std::string desc = (*itError).description;
      long int pos = desc.find("&",0);
      while(pos != -1)
        {
        desc.replace(pos,1,"&amp;");
        pos = desc.find("&",pos+3);
        }
      pos = desc.find("<");
      while(pos != -1)
        {
        desc.replace(pos,1,"&#x03C;");
        pos = desc.find("<");
        }
      pos = desc.find(">");
      while(pos != -1)
        {
        desc.replace(pos,1,"&#x03E;");
        pos = desc.find(">");
        }

      desc += " ["+(*it).GetTestDescription((*itError).number)+"] (";
      char* val = new char[255];
      sprintf(val,"%d",(*itError).line);
      desc += val;
      desc += ")\n";
      delete [] val; 
      
      file << desc;
      
      if(!group)
        {
        file << "</Text>" << std::endl;
        // Show the actual error in the precontext
        file << "          <PreContext>";
        //file << (*it).GetTestDescription((*itError).number) << std::endl;
        file << "</PreContext>" << std::endl;
        file << "<PostContext>";
        file << "</PostContext>" << std::endl;
        file << "<RepeatCount>0</RepeatCount>" << std::endl;
        file << "</Error>" << std::endl;
        }
      itError++;
      nErrors++;
      if((maxError != -1) && (nErrors >= maxError))
        {
        break;
        }
      }
  
    if(group && !first)
      {
      file << "</Text>" << std::endl;
      // Show the actual error in the precontext
      file << "          <PreContext>";
      //file << (*it).GetTestDescription((*itError).number) << std::endl;
      file << "</PreContext>" << std::endl;
      file << "<PostContext>";
      file << "</PostContext>" << std::endl;
      file << "<RepeatCount>0</RepeatCount>" << std::endl;
      file << "</Error>" << std::endl;
      }
   
    if((maxError != -1) && (nErrors >= maxError))
      {
      break;
      }
    it++;
    }
 
  // Write the footer
  file << " <Log Encoding=\"base64\" Compression=\"/bin/gzip\">" << std::endl;
  file << "      </Log>" << std::endl;
  file << "      <EndDateTime>";
  file << kwssys::SystemTools::GetCurrentDateTime("%b %d %I:%M:%S %z");
  file << "</EndDateTime>" << std::endl;

  double time1 = kwssys::SystemTools::GetTime();

  char* timestr = new char[10];
  sprintf(timestr,"%.1f",(time1-time)/60.0);

  file << "  <ElapsedMinutes>" << timestr << "</ElapsedMinutes></Build>" << std::endl;
  file << "</Site>" << std::endl;

  delete [] timestr;
  configfile.close();
  file.close();
  std::cout << "Done." << std::endl;
  return true;
 }

} // end namespace kws
