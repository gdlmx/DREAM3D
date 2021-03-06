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

#include "UncertainRegularGridSampleSurfaceMesh.h"

#include <QtCore/QMap>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "DREAM3DLib/DataContainers/DynamicListArray.hpp"

#include "DREAM3DLib/Utilities/DREAM3DRandom.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UncertainRegularGridSampleSurfaceMesh::UncertainRegularGridSampleSurfaceMesh() :
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_XPoints(0),
  m_YPoints(0),
  m_ZPoints(0),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL)
{
  m_Resolution.x = 1.0f;
  m_Resolution.y = 1.0f;
  m_Resolution.z = 1.0f;
  m_Uncertainty.x = 0.1f;
  m_Uncertainty.y = 0.1f;
  m_Uncertainty.z = 0.1f;
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
UncertainRegularGridSampleSurfaceMesh::~UncertainRegularGridSampleSurfaceMesh()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::setupFilterParameters()
{
  FilterParameterVector parameters = getFilterParameters();
  parameters.push_back(FilterParameter::New("X Points (Voxels)", "XPoints", FilterParameterWidgetType::IntWidget, getXPoints(), false, "Column"));
  parameters.push_back(FilterParameter::New("Y Points (Voxels)", "YPoints", FilterParameterWidgetType::IntWidget, getYPoints(), false, "Row"));
  parameters.push_back(FilterParameter::New("Z Points (Voxels)", "ZPoints", FilterParameterWidgetType::IntWidget, getZPoints(), false, "Plane"));
  parameters.push_back(FilterParameter::New("Resolution", "Resolution", FilterParameterWidgetType::FloatVec3Widget, getResolution(), false, "Microns"));
  parameters.push_back(FilterParameter::New("Uncertainty", "Uncertainty", FilterParameterWidgetType::FloatVec3Widget, getUncertainty(), false, "Microns"));
  parameters.push_back(FilterParameter::New("Created Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("New DataContainer Name", "DataContainerName", FilterParameterWidgetType::StringWidget, getDataContainerName(), true, ""));
  parameters.push_back(FilterParameter::New("New Cell Attribute Matrix Name", "CellAttributeMatrixName", FilterParameterWidgetType::StringWidget, getCellAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("FeatureIds", "FeatureIdsArrayName", FilterParameterWidgetType::StringWidget, getFeatureIdsArrayName(), true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  SampleSurfaceMesh::readFilterParameters(reader, index);
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName() ) );
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName() ) );
  setFeatureIdsArrayName(reader->readString("FeatureIdsArrayName", getFeatureIdsArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int UncertainRegularGridSampleSurfaceMesh::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  SampleSurfaceMesh::writeFilterParameters(writer, index);
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(DataContainerName)
  DREAM3D_FILTER_WRITE_PARAMETER(CellAttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(FeatureIdsArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::dataCheck()
{
  DataArrayPath tempPath;
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0) { return; }

  QVector<size_t> dims(1, 1);
  tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getFeatureIdsArrayName() );
  m_FeatureIdsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  SampleSurfaceMesh::preflight();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  DREAM3D_RANDOMNG_NEW()

  // set volume datacontainer details
  m->setDimensions(m_XPoints, m_YPoints, m_ZPoints);
  m->setOrigin(0.0, 0.0, 0.0);
  m->setResolution(m_Resolution.x, m_Resolution.y, m_Resolution.z);

  SampleSurfaceMesh::execute();

  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VertexArray::Pointer UncertainRegularGridSampleSurfaceMesh::generate_points()
{
  VertexArray::Pointer points = VertexArray::CreateArray((m_XPoints * m_YPoints * m_ZPoints), "points");

  DREAM3D_RANDOMNG_NEW()

  int count = 0;
  float coords[3];
  float lastX = 0, lastY = 0, lastZ = 0;
  for(int k = 0; k < m_ZPoints; k++)
  {
    float randomZ = 2 * static_cast<float>(rg.genrand_res53()) - 1;
    for(int j = 0; j < m_YPoints; j++)
    {
      float randomY = 2 * static_cast<float>(rg.genrand_res53()) - 1;
      for(int i = 0; i < m_XPoints; i++)
      {
        float randomX = 2 * static_cast<float>(rg.genrand_res53()) - 1;
        coords[0] = lastX + m_Resolution.x + (m_Uncertainty.x * randomX);
        coords[1] = lastY + m_Resolution.y + (m_Uncertainty.y * randomY);
        coords[2] = lastZ + m_Resolution.z + (m_Uncertainty.z * randomZ);
        points->setCoords(count, coords);
        lastX = coords[0];
        lastY = coords[1];
        lastZ = coords[2];
        count++;
      }
    }
  }

  return points;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void UncertainRegularGridSampleSurfaceMesh::assign_points(Int32ArrayType::Pointer iArray)
{
  int32_t* ids = iArray->getPointer(0);
  for(int i = 0; i < (m_XPoints * m_YPoints * m_ZPoints); i++)
  {
    m_FeatureIds[i] = ids[i];
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer UncertainRegularGridSampleSurfaceMesh::newFilterInstance(bool copyFilterParameters)
{
  UncertainRegularGridSampleSurfaceMesh::Pointer filter = UncertainRegularGridSampleSurfaceMesh::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UncertainRegularGridSampleSurfaceMesh::getCompiledLibraryName()
{ return Sampling::SamplingBaseName; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UncertainRegularGridSampleSurfaceMesh::getGroupName()
{ return DREAM3D::FilterGroups::SamplingFilters; }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString UncertainRegularGridSampleSurfaceMesh::getHumanLabel()
{ return "Sample Surface Mesh on Uncertain Regular Grid"; }

