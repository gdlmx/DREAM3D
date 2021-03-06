/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "PhReader.h"

#include <stdio.h>

#include <QtCore/QtDebug>
#include <fstream>
#include <sstream>

#include <QtCore/QFileInfo>

#include "DREAM3DLib/DataArrays/DataArray.hpp"

#include "IO/IOConstants.h"

#define BUF_SIZE 1024

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::PhReader() :
  FileReader(),
  m_VolumeDataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_InputFile(""),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  m_Origin.x = 0.0;
  m_Origin.y = 0.0;
  m_Origin.z = 0.0;

  m_Resolution.x = 1.0;
  m_Resolution.y = 1.0;
  m_Resolution.z = 1.0;

  m_Dims[0] = 0;
  m_Dims[1] = 0;
  m_Dims[2] = 0;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhReader::~PhReader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FileSystemFilterParameter::New("Input File", "InputFile", FilterParameterWidgetType::InputFileWidget, getInputFile(), false, "", "*.ph", "CMU Grain Growth"));
  parameters.push_back(FilterParameter::New("Origin", "Origin", FilterParameterWidgetType::FloatVec3Widget, getOrigin(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "XYZ"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("Volume Data Container", "VolumeDataContainerName", FilterParameterWidgetType::StringWidget, getVolumeDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("Cell Attribute Matrix", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setVolumeDataContainerName(reader->readString("VolumeDataContainerName", getVolumeDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  setInputFile( reader->readString( "InputFile", getInputFile() ) );
  setOrigin( reader->readFloatVec3("Origin", getOrigin() ) );
  setResolution( reader->readFloatVec3("Resolution", getResolution() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhReader::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(VolumeDataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(InputFile)
  DREAM3D_FILTER_WRITE_PARAMETER(Origin)
  DREAM3D_FILTER_WRITE_PARAMETER(Resolution)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::updateCellInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->createNonPrereqDataContainer<VolumeDataContainer, PhReader>(this, getVolumeDataContainerName());
  if(getErrorCondition() < 0) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer attrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  QFileInfo fi(getInputFile());
  if (getInputFile().isEmpty() == true)
  {
    QString ss = QObject::tr("%1 needs the Input File Set and it was not.").arg(ClassName());
    setErrorCondition(-387);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else if (fi.exists() == false)
  {
    QString ss = QObject::tr("The input file does not exist");
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  tempPath.update(getVolumeDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  if (getInputFile().isEmpty() == false && fi.exists() == true)
  {
    // We need to read the header of the input file to get the dimensions
    m_InStream = fopen(getInputFile().toLatin1().data(), "r");
    if(m_InStream == NULL)
    {
      setErrorCondition(-48802);
      notifyErrorMessage(getHumanLabel(), "Error opening input file", getErrorCondition());
      return;
    }
    int error = readHeader();
    fclose(m_InStream);
    m_InStream = NULL;
    if (error < 0)
    {
      setErrorCondition(error);
      QString ss = QObject::tr("Error occurred trying to parse the dimensions from the input file. Is the input file a Ph file?");
      notifyErrorMessage(getHumanLabel(), ss, -48010);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhReader::execute()
{
  int err = 0;

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  m_InStream = fopen(getInputFile().toLatin1().data(), "r");
  if(m_InStream == NULL)
  {
    setErrorCondition(-48030);
    notifyErrorMessage(getHumanLabel(), "Error opening input file", getErrorCondition());
    return;
  }

  err = readHeader();
  if(err < 0)
  {
    fclose(m_InStream);
    m_InStream = NULL;
    return;
  }
  err = readFile();
  fclose(m_InStream);
  m_InStream = NULL;
  if(err < 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int PhReader::readHeader()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getVolumeDataContainerName());

  int nx = 0;
  int ny = 0;
  int nz = 0;


  // Read Line #1 which has the dimensions
  fscanf(m_InStream, "%d %d %d\n", &nx, &ny, &nz);
  m->setDimensions(nx, ny, nz);

  char buf[BUF_SIZE];
  // Read Line #2 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, m_InStream);
  // Read Line #3 and dump it
  ::memset(buf, 0, BUF_SIZE);
  fgets(buf, BUF_SIZE, m_InStream);
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  PhReader::readFile()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getVolumeDataContainerName());

  size_t totalPoints = m->getTotalPoints();

  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getXPoints();
  tDims[1] = m->getYPoints();
  tDims[2] = m->getZPoints();
  m->getAttributeMatrix(getCellAttributeMatrixName())->resizeAttributeArrays(tDims);
  updateCellInstancePointers();

  for(size_t n = 0; n < totalPoints; ++n)
  {
    if (fscanf(m_InStream, "%d", m_FeatureIds + n) == 0)
    {
      fclose(m_InStream);
      m_InStream = NULL;
      setErrorCondition(-48040);
      notifyErrorMessage(getHumanLabel(), "Error reading Ph data", getErrorCondition());
      return getErrorCondition();
    }
  }

  // Now set the Resolution and Origin that the user provided on the GUI or as parameters
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);
  m->setOrigin(m_Origin.x, m_Origin.y, m_Origin.z);

  notifyStatusMessage(getHumanLabel(), "Complete");
  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer PhReader::newFilterInstance(bool copyFilterParameters)
{
  PhReader::Pointer filter = PhReader::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getCompiledLibraryName()
{ return IO::IOBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getGroupName()
{ return DREAM3D::FilterGroups::IOFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getSubGroupName()
{ return DREAM3D::FilterSubGroups::InputFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString PhReader::getHumanLabel()
{ return "Read Ph File (Feature Ids)"; }

