/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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


#include "SharedFeatureFaceFilter.h"




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::SharedFeatureFaceFilter() :
  SurfaceMeshFilter(),
  m_SurfaceDataContainerName(DREAM3D::Defaults::SurfaceDataContainerName),
  m_FaceAttributeMatrixName(DREAM3D::Defaults::FaceAttributeMatrixName),
  m_SurfaceMeshFeatureFaceIdArrayName(DREAM3D::FaceData::SurfaceMeshFeatureFaceId),
  m_SurfaceMeshFaceLabelsArrayName(DREAM3D::FaceData::SurfaceMeshFaceLabels),
  m_SurfaceMeshFaceLabels(NULL),
/*[]*/m_SurfaceMeshFaceLabelsArrayPath(DREAM3D::Defaults::SomePath)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::~SharedFeatureFaceFilter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::setupFilterParameters()
{
  FilterParameterVector parameters;
/*[]*/parameters.push_back(FilterParameter::New("SurfaceMeshFaceLabels", "SurfaceMeshFaceLabelsArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, "DataArrayPath", true, ""));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSurfaceMeshFaceLabelsArrayPath(reader->readDataArrayPath("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int SharedFeatureFaceFilter::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("SurfaceMeshFaceLabelsArrayPath", getSurfaceMeshFaceLabelsArrayPath() );
  /* Place code that will write the inputs values into a file. reference the
   AbstractFilterParametersWriter class for the proper API to use. */
  /*  writer->writeValue("OutputFile", getOutputFile() ); */
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::dataCheck()
{
  SurfaceDataContainer* sm = getDataContainerArray()->getPrereqDataContainer<SurfaceDataContainer, AbstractFilter>(this, getSurfaceDataContainerName(), false);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer faceAttrMat = sm->getPrereqAttributeMatrix<AbstractFilter>(this, getFaceAttributeMatrixName(), -301);
  if(getErrorCondition() < 0) { return; }

  if(NULL == sm)
  {
    setErrorCondition(-10000);
    notifyErrorMessage(getHumanLabel(), "Surface Data Container is NULL", getErrorCondition());
    return;
  }
  // We MUST have Nodes
  if(sm->getVertices().get() == NULL)
  {
    setErrorCondition(-384);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Nodes", getErrorCondition());
  }

  // We MUST have Triangles defined also.
  if(sm->getFaces().get() == NULL)
  {
    setErrorCondition(-385);
    notifyErrorMessage(getHumanLabel(), "SurfaceMesh DataContainer missing Triangles", getErrorCondition());
  }

  QVector<size_t> dims(1, 2);
////====>REMOVE THIS    m_SurfaceMeshFaceLabelsPtr = faceAttrMat->getPrereqArray<DataArray<int32_t>, AbstractFilter>(this, m_SurfaceMeshFaceLabelsArrayName, -386, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  m_SurfaceMeshFaceLabelsPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter>(this, getSurfaceMeshFaceLabelsArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_SurfaceMeshFaceLabelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_SurfaceMeshFaceLabels = m_SurfaceMeshFaceLabelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  // List any arrays that are created during this filter
  dims[0] = 1;
  Int32ArrayType::Pointer featureFaceId = Int32ArrayType::CreateArray(1, dims, DREAM3D::FaceData::SurfaceMeshFeatureFaceId);
  sm->getAttributeMatrix(getFaceAttributeMatrixName())->addAttributeArray(DREAM3D::FaceData::SurfaceMeshFeatureFaceId, featureFaceId);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SharedFeatureFaceFilter::execute()
{
  int err = 0;
  setErrorCondition(err);
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  SurfaceDataContainer* sm = getDataContainerArray()->getDataContainerAs<SurfaceDataContainer>(getSurfaceDataContainerName());
  notifyStatusMessage(getHumanLabel(), "Starting");

  /* Place all your code to execute your filter here. */

  FaceArray::Pointer trianglesPtr = sm->getFaces();
//  FaceArray::Face_t* triangles = trianglesPtr->getPointer(0);
  size_t totalPoints = trianglesPtr->getNumberOfTuples();

  Int32ArrayType::Pointer featureFaceId = Int32ArrayType::CreateArray(trianglesPtr->getNumberOfTuples(), DREAM3D::FaceData::SurfaceMeshFeatureFaceId);
  featureFaceId->initializeWithZeros();

  QMap<uint64_t, int> faceSizeMap;
  QMap<uint64_t, int32_t> faceIdMap; // This maps a unique 64 bit integer to an increasing 32 bit integer
  uint32_t index = 0;
  struct { int g; int r; } faceId;
  uint64_t* faceId_64 = reinterpret_cast<uint64_t*>(&faceId);
  int32_t fl0 = -1;
  int32_t fl1 = -1;
  // Loop through all the Triangles and figure out how many triangles we have in each one.
  for(size_t t = 0; t < totalPoints; ++t)
  {
    fl0 = m_SurfaceMeshFaceLabels[t * 2];
    fl1 = m_SurfaceMeshFaceLabels[t * 2 + 1];
    if (fl0 < fl1)
    {
      faceId.g = fl0;
      faceId.r = fl1;
    }
    else
    {
      faceId.g = fl1;
      faceId.r = fl0;
    }

    QMap<uint64_t, int>::iterator iter = faceSizeMap.find(*faceId_64);
    if(iter == faceSizeMap.end())
    {
      faceSizeMap[*faceId_64] = 1;
      faceIdMap[*faceId_64] = index;
      featureFaceId->setValue(t, index);
      ++index;
    }
    else
    {
      faceSizeMap[*faceId_64]++;
      featureFaceId->setValue(t, faceIdMap[*faceId_64]);
    }
  }

  SharedFeatureFaces_t faces;

  // Allocate all the vectors that we need
  for(QMap<uint64_t, int>::iterator iter = faceSizeMap.begin(); iter != faceSizeMap.end(); ++iter)
  {
    FaceIds_t v;
    v.reserve(iter.value());
    index = faceIdMap[iter.key()];
    faces[index] = v;
  }

  // Loop through all the Triangles and assign each one to a unique Feature Face Id.
  for(size_t t = 0; t < totalPoints; ++t)
  {
#if 0
    Triangle& tri = triangles[t];
    if (tri.nSpin[0] < tri.nSpin[1])
    {
      faceId.g = tri.nSpin[0];
      faceId.r = tri.nSpin[1];
    }
    else
    {
      faceId.g = tri.nSpin[1];
      faceId.r = tri.nSpin[0];
    }
    index = faceIdMap[*faceId_64];
#else
    index = featureFaceId->getValue(t);
#endif
    faces[index].push_back(t);
  }

  m_SharedFeatureFaces = faces;

  sm->getAttributeMatrix(getFaceAttributeMatrixName())->addAttributeArray(DREAM3D::FaceData::SurfaceMeshFeatureFaceId, featureFaceId);

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SharedFeatureFaceFilter::SharedFeatureFaces_t& SharedFeatureFaceFilter::getSharedFeatureFaces()
{
  return m_SharedFeatureFaces;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer SharedFeatureFaceFilter::newFilterInstance(bool copyFilterParameters)
{
  SharedFeatureFaceFilter::Pointer filter = SharedFeatureFaceFilter::New();
  if(true == copyFilterParameters)
  {
    filter->setSurfaceMeshFaceLabelsArrayPath(getSurfaceMeshFaceLabelsArrayPath());
  }
  return filter;
}
