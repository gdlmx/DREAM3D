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

#include "ScalarSegmentFeatures.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "DREAM3DLib/Common/Constants.h"

#include "DREAM3DLib/OrientationOps/OrientationOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"

#define ERROR_TXT_OUT 1
#define ERROR_TXT_OUT1 1




#define NEW_SHARED_ARRAY(var, m_msgType, size)\
  boost::shared_array<m_msgType> var##Array(new m_msgType[size]);\
  m_msgType* var = var##Array.get();



/* from http://www.newty.de/fpt/functor.html */
// base class
class CompareFunctor
{
  public:
    virtual ~CompareFunctor() {}

    virtual bool operator()(size_t index, size_t neighIndex, size_t gnum)  // call using operator
    {
      return false;
    }
};


template<class T>
class TSpecificCompareFunctor : public CompareFunctor
{
  public:
    TSpecificCompareFunctor(void* data, size_t length, T tolerance, int32_t* featureIds) :
      m_Length(length),
      m_Tolerance(tolerance),
      m_FeatureIds(featureIds)
    {
      m_Data = reinterpret_cast<T*>(data);
    }
    virtual ~TSpecificCompareFunctor() {};

    virtual bool operator()(size_t referencepoint, size_t neighborpoint, size_t gnum)
    {
      // Sanity check the indices that are being passed in.
      if (referencepoint >= m_Length || neighborpoint >= m_Length) { return false; }

      if(m_Data[referencepoint] >= m_Data[neighborpoint])
      {
        if ((m_Data[referencepoint] - m_Data[neighborpoint]) <= m_Tolerance)
        {
          m_FeatureIds[neighborpoint] = gnum;
          return true;
        }
      }
      else
      {
        if ((m_Data[neighborpoint] - m_Data[referencepoint]) <= m_Tolerance)
        {
          m_FeatureIds[neighborpoint] = gnum;
          return true;
        }
      }
      return false;
    }

  protected:
    TSpecificCompareFunctor() {}

  private:
    T* m_Data;       // The data that is being compared
    size_t m_Length; // Length of the Data Array
    T      m_Tolerance; // The tolerance of the comparison
    int32_t* m_FeatureIds; // The feature Ids
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentFeatures::ScalarSegmentFeatures() :
  SegmentFeatures(),
  m_DataContainerName(DREAM3D::Defaults::VolumeDataContainerName),
  m_CellFeatureAttributeMatrixName(DREAM3D::Defaults::CellFeatureAttributeMatrixName),
  m_CellAttributeMatrixName(DREAM3D::Defaults::CellAttributeMatrixName),
  m_ScalarArrayName(""),
  m_ScalarTolerance(5.0f),
  m_RandomizeFeatureIds(true),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_FeatureIds(NULL),
  m_GoodVoxelsArrayName(DREAM3D::CellData::GoodVoxels),
  m_GoodVoxels(NULL),
  m_ActiveArrayName(DREAM3D::FeatureData::Active),
  m_Active(NULL),
  m_Compare(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ScalarSegmentFeatures::~ScalarSegmentFeatures()
{
  if (m_Compare != NULL)
  {
    delete m_Compare;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Input Cell Array Name");
    parameter->setPropertyName("ScalarArrayName");
    parameter->setWidgetType(FilterParameterWidgetType::SingleArraySelectionWidget);
    parameter->setValueType("QString");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setPropertyName("ScalarTolerance");
    parameter->setHumanLabel("Scalar Tolerance");
    parameter->setWidgetType(FilterParameterWidgetType::DoubleWidget);
    parameter->setValueType("float");
    parameter->setCastableValueType("double");
    parameter->setUnits("");
    parameters.push_back(parameter);
  }
#if 0
  {
    FilterParameter::Pointer parameter = FilterParameter::New();
    parameter->setHumanLabel("Randomly Reorder Generated Feature Ids");
    parameter->setPropertyName("RandomizeFeatureIds");
    parameter->setWidgetType(FilterParameterWidgetType::BooleanWidget);
    parameter->setValueType("bool");
    parameters.push_back(parameter);
  }
#endif
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setScalarArrayName( reader->readString( "ScalarArrayName", getScalarArrayName() ) );
  setScalarTolerance( reader->readValue("ScalarTolerance", getScalarTolerance()) );
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ScalarSegmentFeatures::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("ScalarArrayName", getScalarArrayName() );
  writer->writeValue("ScalarTolerance", getScalarTolerance() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::updateFeatureInstancePointers()
{
  setErrorCondition(0);

  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::dataCheck()
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }
  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer cellFeatureAttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getCellFeatureAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }
  AttributeMatrix::Pointer cellAttrMat = m->getPrereqAttributeMatrix<AbstractFilter>(this, getCellAttributeMatrixName(), -301);
  if(getErrorCondition() < 0 || NULL == cellAttrMat.get() ) { return; }

  if(m_ScalarArrayName.isEmpty() == true)
  {
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), "An array from the Volume DataContainer must be selected.", getErrorCondition());
  }

  QVector<size_t> dims(1, 1);
  m_FeatureIdsPtr = cellAttrMat->createNonPrereqArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, m_FeatureIdsArrayName, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_GoodVoxelsPtr = cellAttrMat->getPrereqArray<DataArray<bool>, AbstractFilter>(NULL, m_GoodVoxelsArrayName, -304, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_GoodVoxelsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_GoodVoxels = m_GoodVoxelsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  else {m_GoodVoxels = NULL;}
  m_ActivePtr = cellFeatureAttrMat->createNonPrereqArray<DataArray<bool>, AbstractFilter, bool>(this, m_ActiveArrayName, true, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_ActivePtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_Active = m_ActivePtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::execute()
{
  setErrorCondition(0);

  dataCheck();
  if(getErrorCondition() < 0) { return; }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<size_t> tDims(1, 1);
  m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
  // This runs a subfilter
  int64_t totalPoints = m->getAttributeMatrix(getCellAttributeMatrixName())->getNumTuples();

  QString dcName;
  QString amName;
  QString daName;

  QStringList tokens = m_ScalarArrayName.split(DREAM3D::PathSep);
  // We should end up with 3 Tokens
  if(tokens.size() != 3)
  {
    setErrorCondition(-11002);
    QString ss = QObject::tr("The path to the Attribute Array is malformed. Each part should be separated by a '|' character.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else
  {
    dcName = tokens.at(0);
    amName = tokens.at(1);
    daName = tokens.at(2);

    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
    if(NULL == dc.get())
    {
      setErrorCondition(-11003);
      QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
      if(NULL == attrMat.get())
    {
      setErrorCondition(-11004);
      QString ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
  }

  m_InputData = m->getAttributeMatrix(getCellAttributeMatrixName())->getAttributeArray(daName);
  if (NULL == m_InputData.get())
  {

    QString ss = QObject::tr("Selected array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_ScalarArrayName);
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Tell the user we are starting the filter
  notifyStatusMessage(getHumanLabel(), "Starting");

  for(int64_t i = 0; i < totalPoints; i++)
  {
    m_FeatureIds[i] = 0;
  }

  QString dType = m_InputData->getTypeAsString();
  if(m_InputData->getNumberOfComponents() != 1)
  {
    m_Compare = new CompareFunctor(); // The default CompareFunctor which ALWAYS returns false for the comparison
  }
  else if (dType.compare("int8_t") == 0)
  {
    m_Compare = new TSpecificCompareFunctor<int8_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint8_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint8_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("bool") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<bool>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("int16_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int16_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint16_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint16_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("int32_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int32_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint32_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint32_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("int64_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<int64_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("uint64_t") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<uint64_t>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("float") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<float>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }
  else if (dType.compare("double") == 0)
  {
    m_Compare =  new TSpecificCompareFunctor<double>(m_InputData->getVoidPointer(0), m_InputData->getNumberOfTuples(), m_ScalarTolerance, m_FeatureIds);
  }

  missingGoodVoxels = true;
  if (NULL != m_GoodVoxels)
  {
    missingGoodVoxels = false;
  }

  // Generate the random voxel indices that will be used for the seed points to start a new grain growth/agglomeration
  const size_t rangeMin = 0;
  const size_t rangeMax = totalPoints - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

  SegmentFeatures::execute();

  size_t totalFeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();
  if (totalFeatures < 2)
  {
    setErrorCondition(-87000);
    notifyErrorMessage(getHumanLabel(), "The number of Features was 0 or 1 which means no features were detected. Is a threshold value set to high?", getErrorCondition());
    return;
  }

  // By default we randomize grains
  if (true == m_RandomizeFeatureIds)
  {
    totalPoints = m->getTotalPoints();
    randomizeFeatureIds(totalPoints, totalFeatures);
  }
  // If there is an error set this to something negative and also set a message
  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::randomizeFeatureIds(int64_t totalPoints, size_t totalFeatures)
{
  notifyStatusMessage(getHumanLabel(), "Randomizing Feature Ids");
  // Generate an even distribution of numbers between the min and max range
  const size_t rangeMin = 0;
  const size_t rangeMax = totalFeatures - 1;
  initializeVoxelSeedGenerator(rangeMin, rangeMax);

// Get a reference variable to the Generator object
  Generator& numberGenerator = *m_NumberGenerator;

  DataArray<int32_t>::Pointer rndNumbers = DataArray<int32_t>::CreateArray(totalFeatures, "New GrainIds");

  int32_t* gid = rndNumbers->getPointer(0);
  gid[0] = 0;
  for(size_t i = 1; i < totalFeatures; ++i)
  {
    gid[i] = i;
  }

  size_t r;
  size_t temp;
  //--- Shuffle elements by randomly exchanging each with one other.
  for (size_t i = 1; i < totalFeatures; i++)
  {
    r = numberGenerator(); // Random remaining position.
    if (r >= totalFeatures) {
      continue;
    }
    temp = gid[i];
    gid[i] = gid[r];
    gid[r] = temp;
  }

  // Now adjust all the Grain Id values for each Voxel
  for(int64_t i = 0; i < totalPoints; ++i)
  {
    m_FeatureIds[i] = gid[ m_FeatureIds[i] ];
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int64_t ScalarSegmentFeatures::getSeed(size_t gnum)
{
  setErrorCondition(0);
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  int64_t totalPoints = m->getTotalPoints();
  int seed = -1;
  Generator& numberGenerator = *m_NumberGenerator;
  while(seed == -1 && m_TotalRandomNumbersGenerated < totalPoints)
  {
    // Get the next voxel index in the precomputed list of voxel seeds
    size_t randpoint = numberGenerator();
    m_TotalRandomNumbersGenerated++; // Increment this counter
    if(m_FeatureIds[randpoint] == 0) // If the GrainId of the voxel is ZERO then we can use this as a seed point
    {
      if ((missingGoodVoxels == true || m_GoodVoxels[randpoint] == true))
      {
        seed = randpoint;
      }
    }
  }
  if (seed >= 0)
  {
    m_FeatureIds[seed] = gnum;
    QVector<size_t> tDims(1, gnum+1);
    m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->resizeAttributeArrays(tDims);
    updateFeatureInstancePointers();
  }
  return seed;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ScalarSegmentFeatures::determineGrouping(int64_t referencepoint, int64_t neighborpoint, size_t gnum)
{

  if(m_FeatureIds[neighborpoint] == 0 && (m_GoodVoxels[neighborpoint] == true || missingGoodVoxels == true))
  {
    return (*m_Compare)( (size_t)(referencepoint), (size_t)(neighborpoint), gnum );
  //     | Functor  ||calling the operator() method of the CompareFunctor Class |
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ScalarSegmentFeatures::initializeVoxelSeedGenerator(const size_t rangeMin, const size_t rangeMax)
{

// The way we are using the boost random number generators is that we are asking for a NumberDistribution (see the typedef)
// to guarantee the numbers are betwee a specific range and will only be generated once. We also keep a tally of the
// total number of numbers generated as a way to make sure the while loops eventually terminate. This setup should
// make sure that every voxel can be a seed point.
//  const size_t rangeMin = 0;
//  const size_t rangeMax = totalPoints - 1;
  m_Distribution = boost::shared_ptr<NumberDistribution>(new NumberDistribution(rangeMin, rangeMax));
  m_RandomNumberGenerator = boost::shared_ptr<RandomNumberGenerator>(new RandomNumberGenerator);
  m_NumberGenerator = boost::shared_ptr<Generator>(new Generator(*m_RandomNumberGenerator, *m_Distribution));
  m_RandomNumberGenerator->seed(static_cast<size_t>( QDateTime::currentMSecsSinceEpoch() )); // seed with the current time
  m_TotalRandomNumbersGenerated = 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ScalarSegmentFeatures::newFilterInstance(bool copyFilterParameters)
{
  /*
  * ScalarArrayName
  * ScalarTolerance
  */
  ScalarSegmentFeatures::Pointer filter = ScalarSegmentFeatures::New();
  if(true == copyFilterParameters)
  {
    filter->setScalarArrayName( getScalarArrayName() );
    filter->setScalarTolerance( getScalarTolerance() );
  }
  return filter;
}