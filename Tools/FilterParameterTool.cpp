/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <iostream>


#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QMetaProperty>

// DREAM3DLib includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DREAM3DVersion.h"


#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/FilterFactory.hpp"

#include "DREAM3DLib/Plugin/DREAM3DPluginInterface.h"
#include "DREAM3DLib/Plugin/DREAM3DPluginLoader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString quote(const QString& str)
{
  return QString("\"%1\"").arg(str);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeOutput(bool didReplace, QStringList &outLines, QString filename)
{
  if(didReplace == true)
  {
    QFileInfo fi2(filename);
#if 1
    QFile hOut(filename);
#else
    QString tmpPath = "/tmp/" + fi2.fileName();
    QFile hOut(tmpPath);
#endif
    hOut.open(QFile::WriteOnly);
    QTextStream stream( &hOut );
    stream << outLines.join("\n");
    hOut.close();

    qDebug() << "Saved File " << fi2.absoluteFilePath();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createReplacementDataCheck(QStringList &outLines, QString &line, QString searchString)
{

  qDebug() << "Found a Prereq Array";
  // write out the old line commented out
  outLines.push_back(QString("////====>REMOVE THIS  ") + line);

  int offset = line.indexOf(">(");
  offset = offset + 2;
  offset = line.indexOf(',', offset) + 1; // Find the first comma of the argument list
  int offset2 = line.indexOf(',', offset + 1); // find the next comma. This should bracket the 2nd argument
  QString arg = line.mid(offset, (offset2 - offset)).trimmed();
  QString arrayName = arg;
  arg = arg.mid(2);
  offset = arg.indexOf("ArrayName");
  arg = arg.mid(0, offset);
  offset = arg.indexOf('.');
  if (offset > 0)
  {
    arg = arg.mid(0, offset);
  }
  qDebug() << arg;


  // Extract out the entire argument as a string
  offset = line.indexOf(">(") + 1;
  QString right = line.mid(offset);
  right.replace(arrayName, "get" + arg + "ArrayPath()" );
  QStringList tokens = right.split(',');
  right = tokens[0] + "," + tokens[1] + "," + tokens[3];

  // Extract the type of Array
  offset = line.indexOf("->getPrereqArray<") + searchString.size();
  offset2 = line.indexOf(',', offset);
  QString type = line.mid(offset, offset2 - offset);
  qDebug() << type;

  QString buf;
  QTextStream out(&buf);

  out << "  m_" << arg << "Ptr = getDataContainerArray()->getPrereqArrayFromPath<" << type << ", AbstractFilter>" << right;
  qDebug() << buf;
  outLines.push_back(buf);
  return arg;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createReplacementReader(QStringList &outLines, QString name)
{
  QString str;
  QTextStream out(&str);
  out << "  set" << name << "ArrayPath(reader->readDataArrayPath(\"" << name << "ArrayPath\", get" << name << "ArrayPath() ) );";
  outLines.push_back(str);
  return "";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createReplacementWriter(QStringList &outLines, QString name)
{
  QString str;
  QTextStream out(&str);
  out << "  writer->writeValue(\"" << name << "ArrayPath\", get" << name << "ArrayPath() );";
  outLines.push_back(str);
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createNewFilterInstance(QStringListIterator &sourceLines, QStringList &outLines, QString name)
{
  QString line = sourceLines.next();
  outLines.push_back(line);

  line = sourceLines.next();
  // Eat up the comment block
  if(line.contains("/*") )
  {
    while(sourceLines.hasNext() )
    {
      QString ll = sourceLines.next().trimmed();
      if(ll.startsWith("*/") ) {
        line = sourceLines.next(); break;
      }
    }
  }

  outLines.push_back(line);
  line = sourceLines.next();
  outLines.push_back(line);
  line = sourceLines.next();
  outLines.push_back(line);
  QString str;
  QTextStream out(&str);
  out << "    filter->set" << name << "ArrayPath(get" << name << "ArrayPath());";
  outLines.push_back(str);
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createSetupFilterParameters(QStringListIterator &sourceLines, QStringList &outLines, QString name)
{
  //  parameters.push_back(FilterParameter::New("Input Statistics", "InputStatsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  QString str;
  QTextStream out(&str);
  out << "/*[]*/parameters.push_back(FilterParameter::New(\"" << name << "\", \"" << name << "ArrayPath\", FilterParameterWidgetType::DataArraySelectionWidget, \"DataArrayPath\", true, \"\"));";
  outLines.push_back(str);
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString createConstructorEntries(QStringListIterator &sourceLines, QStringList &outLines, QString name)
{
  QString line = sourceLines.next();
  // Eat up the entries already there

  while(line.contains(",") )
  {
    outLines.push_back(line);
    line = sourceLines.next();
  }

  outLines.push_back(line + ",");



  QString str;
  QTextStream out(&str);
  out << "/*[]*/m_" << name << "ArrayPath(DREAM3D::Defaults::SomePath)";
  outLines.push_back(str);

  return "";
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString updateHeader(QStringList &outLines, QString name)
{
  QString str;
  QTextStream out(&str);
  out << "    DREAM3D_FILTER_PARAMETER(DataArrayPath, " << name << "ArrayPath)\n";
  out << "    Q_PROPERTY(DataArrayPath " << name << "ArrayPath READ get" << name << "ArrayPath WRITE set" << name << "ArrayPath)\n";

  outLines.push_back(str);
  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool fixFile( AbstractFilter::Pointer filter, const QString& hFile, const QString& cppFile)
{
  QString contents;
  {
    // Read the Source File
    QFileInfo fi(cppFile);
//    if (fi.baseName().compare("EBSDSegmentFeatures") != 0)
//    {
//      return false;
//    }

    QFile source(cppFile);
    source.open(QFile::ReadOnly);
    contents = source.readAll();
    source.close();
  }

  QStringList names;
  bool didReplace = false;
  QString searchString = "->getPrereqArray<";
  QStringList outLines;
  QStringList list = contents.split(QRegExp("\\n"));
  QStringListIterator sourceLines(list);
  while (sourceLines.hasNext())
  {
    QString line = sourceLines.next();
    if(line.contains(searchString) ) // we found the filter parameter section
    {
      names.push_back(createReplacementDataCheck(outLines, line, searchString) );
      didReplace = true;
    }
    else
    {
      outLines.push_back(line);
    }
  }


  foreach(QString name, names)
  {
    searchString = "reader->openFilterGroup(this, index);";
    list = outLines;
    sourceLines = QStringListIterator(list);
    outLines.clear();
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) ) // we found the filter parameter section
      {
        outLines.push_back(line);
        createReplacementReader(outLines, name);
        didReplace = true;
      }
      else
      {
        outLines.push_back(line);
      }
    }

    searchString = "writer->openFilterGroup(this, index);";
    list = outLines;
    sourceLines = QStringListIterator(list);
    outLines.clear();
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) ) // we found the filter parameter section
      {
        outLines.push_back(line);
        createReplacementWriter(outLines, name);
        didReplace = true;
      }
      else
      {
        outLines.push_back(line);
      }
    }


    searchString = "newFilterInstance(bool copyFilterParameters)";
    list = outLines;
    sourceLines = QStringListIterator(list);
    outLines.clear();
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) ) // we found the filter parameter section
      {
        outLines.push_back(line);
        createNewFilterInstance(sourceLines, outLines, name);
        didReplace = true;
      }
      else
      {
        outLines.push_back(line);
      }
    }

    searchString = "setFilterParameters(parameters);";
    list = outLines;
    sourceLines = QStringListIterator(list);
    outLines.clear();
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) ) // we found the filter parameter section
      {
        createSetupFilterParameters(sourceLines, outLines, name);
        outLines.push_back(line);
        didReplace = true;
      }
      else
      {
        outLines.push_back(line);
      }
    }

    searchString = filter->getNameOfClass() + "::" + filter->getNameOfClass();
    list = outLines;
    sourceLines = QStringListIterator(list);
    outLines.clear();
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) ) // we found the filter parameter section
      {
        outLines.push_back(line);
        createConstructorEntries(sourceLines, outLines, name);
        didReplace = true;
      }
      else
      {
        outLines.push_back(line);
      }
    }

    writeOutput(didReplace, outLines, cppFile);
  }



  foreach(QString name, names)
  {
    /**************** NOW UPDATE THE HEADER FOR THE FILTER ***********************/
    {
      // Read the Source File
      QFileInfo fi(hFile);
      //    if (fi.baseName().compare("FindSizes") != 0)
      //    {
      //      return false;
      //    }

      QFile source(hFile);
      source.open(QFile::ReadOnly);
      contents = source.readAll();
      source.close();
    }
    searchString = "virtual const QString getCompiledLibraryName()";
    list = contents.split(QRegExp("\\n"));;
    sourceLines = QStringListIterator(list);
    outLines.clear();
    while (sourceLines.hasNext())
    {
      QString line = sourceLines.next();
      if(line.contains(searchString) ) // we found the filter parameter section
      {
        updateHeader(outLines, name);
        didReplace = true;
        outLines.push_back(line);
      }
      else
      {
        outLines.push_back(line);
      }
    }

    writeOutput(didReplace, outLines, hFile);
  }
  return didReplace;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString findPath(const QString& groupName, const QString& filtName, const QString ext)
{
  QString prefix("/Users/mjackson/Workspace/DREAM3D_Rewrite/Source/");
  {
    QString path = prefix + "DREAM3DLib/" + groupName + "Filters/" + filtName + ext;
    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }

  prefix = prefix + "Plugins/";
  QStringList libs;
  libs << "DDDAnalysisToolbox" << "ImageImport" << "OrientationAnalysis" << "Processing" <<  "Reconstruction" << "Sampling" << "Statistics"  << "SurfaceMeshing" << "SyntheticBuilding";

  for (int i = 0; i < libs.size(); ++i)
  {
    QString path = prefix + libs.at(i) + "/" + libs.at(i) + "Filters/" + filtName + ext;
    //  std::cout << "****" << path.toStdString() << std::endl;

    QFileInfo fi(path);
    if(fi.exists() == true)
    {
      return path;
    }
  }
  return "NOT FOUND";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateFilterParametersCode()
{

  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();
  QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
  // Loop on each filter
  while(iter.hasNext())
  {
    iter.next();
    IFilterFactory::Pointer factory = iter.value();
    AbstractFilter::Pointer filter = factory->create();

    QString cpp = findPath(filter->getGroupName(), filter->getNameOfClass(), ".cpp");
    QString h = findPath(filter->getGroupName(), filter->getNameOfClass(), ".h");

    fixFile(filter, h, cpp);
  }

}





// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoopOnFilters()
{
  FilterManager::Pointer fm = FilterManager::Instance();
  FilterManager::Collection factories = fm->getFactories();

  FilterManager::CollectionIterator i(factories);
  int count = 0;
  while (i.hasNext())
  {
    i.next();
    std::cout << ++count << ": " << i.key().toStdString() << ": " << std::endl;

    //std::cout << "  public:" << std::endl;
    IFilterFactory::Pointer factory = i.value();
    AbstractFilter::Pointer filter = factory->create();
    //if (filter->getGroupName().compare(DREAM3D::FilterGroups::StatisticsFilters) == 0)
    // if(filter->getNameOfClass().compare("FindSchmids") == 0)
    {
      //   std::cout << "" << filter->getGroupName().toStdString() << "Filters/" << filter->getNameOfClass().toStdString() << ".cpp" << std::endl;
      QString cpp = findPath(filter->getGroupName(), filter->getNameOfClass(), ".cpp");
      std::cout << filter << " " << cpp.toStdString() << std::endl;
    }

  }

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Q_ASSERT(true); // We don't want anyone to run this program.
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("FilterParameterTool");

  std::cout << "FilterParameterTool Starting. Version " << DREAM3DLib::Version::PackageComplete().toStdString() << std::endl;


  // Register all the filters including trying to load those from Plugins
  FilterManager::Pointer fm = FilterManager::Instance();
  DREAM3DPluginLoader::LoadPluginFilters(fm.get());


  // Send progress messages from PipelineBuilder to this object for display
  qRegisterMetaType<PipelineMessage>();

  GenerateFilterParametersCode();

  return 0;
}
