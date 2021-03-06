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

#include "CorrelateValuesWithVectorDirection.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Math/MatrixMath.h"
#include "DREAM3DLib/Math/GeometryMath.h"
#include "OrientationLib/Math/OrientationMath.h"
#include "DREAM3DLib/Common/Constants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CorrelateValuesWithVectorDirection::CorrelateValuesWithVectorDirection() :
  AbstractFilter(),
  m_MaxCoord(sqrt(DREAM3D::Constants::k_2Pi)/2.0),
  m_Dimension(72),
  m_StepSize(sqrt(DREAM3D::Constants::k_2Pi)/72.0),
  m_VectorDataArrayPath(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellAttributeMatrixName, DREAM3D::CellData::VectorData),
  m_VectorDataArrayName(DREAM3D::CellData::VectorData),
  m_VectorData(NULL),
  m_CorrelatedDataArrayPath("", "", "")
{

  setupFilterParameters();  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CorrelateValuesWithVectorDirection::~CorrelateValuesWithVectorDirection()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Required Information", "", FilterParameterWidgetType::SeparatorWidget, "", true));
  parameters.push_back(FilterParameter::New("VectorData", "VectorDataArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getVectorDataArrayPath(), true, ""));
  parameters.push_back(FilterParameter::New("CorrelatedData", "CorrelatedDataArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getCorrelatedDataArrayPath(), true, ""));
  setFilterParameters(parameters);
}


// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* Code to read the values goes between these statements */
  setVectorDataArrayPath(reader->readDataArrayPath("VectorDataArrayPath", getVectorDataArrayPath() ) );
  setCorrelatedDataArrayPath(reader->readDataArrayPath("CorrelatedDataArrayPath", getCorrelatedDataArrayPath() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CorrelateValuesWithVectorDirection::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("VectorDataArrayPath", getVectorDataArrayPath() );
  writer->writeValue("CorrelatedDataArrayPath", getCorrelatedDataArrayPath() );
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QVector<size_t> dims(1, 3);
  m_VectorDataPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getVectorDataArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_VectorDataPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_VectorData = m_VectorDataPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  if(m_CorrelatedDataArrayPath.isEmpty() == true)
  {
    QString ss = QObject::tr("The correlated data array name is empty. Please select a name for the correlated data array");
    setErrorCondition(-11000);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  IDataArray::Pointer inputData = getDataContainerArray()->getDataContainer(m_CorrelatedDataArrayPath.getDataContainerName())->getAttributeMatrix(m_CorrelatedDataArrayPath.getAttributeMatrixName())->getAttributeArray(m_CorrelatedDataArrayPath.getDataArrayName());
  if (NULL == inputData.get())
  {
    QString ss = QObject::tr("Correlated Data array '%1' does not exist in the Voxel Data Container. Was it spelled correctly?").arg(m_CorrelatedDataArrayPath.getDataArrayName());
    setErrorCondition(-11001);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  if(inputData->getNumberOfTuples() != m_VectorDataPtr.lock()->getNumberOfTuples())
  {
    QString ss = QObject::tr("Correlated Data array '%1' has a different number of tuples from the Vector Data array '%2'").arg(m_CorrelatedDataArrayPath.getDataArrayName()).arg(getVectorDataArrayPath().getDataArrayName());
    setErrorCondition(-11002);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::preflight()
{
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void addToLambert(IDataArray::Pointer correlatedData, size_t bin, size_t point, double* m_LambertProjection)
{
  float value = 0.0;
  DataArray<T>* correlatedArray = DataArray<T>::SafePointerDownCast(correlatedData.get());
  if (NULL == correlatedArray)
  {
    return;
  }

  size_t numComps = correlatedArray->getNumberOfComponents();
  T* m_CorrelatedData = correlatedArray->getPointer(0);

  for(int j=0;j<numComps;j++)
  {
    m_LambertProjection[(numComps*bin)+j] += double(m_CorrelatedData[(numComps*point)+j]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::execute()
{
  dataCheck();
  if(getErrorCondition() < 0) { return; }

  IDataArray::Pointer correlatedData = getDataContainerArray()->getDataContainer(m_CorrelatedDataArrayPath.getDataContainerName())->getAttributeMatrix(m_CorrelatedDataArrayPath.getAttributeMatrixName())->getAttributeArray(m_CorrelatedDataArrayPath.getDataArrayName());
  QString dType = correlatedData->getTypeAsString();

  size_t totalPoints = m_VectorDataPtr.lock()->getNumberOfTuples();
  size_t numComps = correlatedData->getNumberOfComponents();

  makeLambertProjection(numComps);

  double* m_LambertProjection = m_LambertProj->getPointer(0);
  QVector<int> counts(m_LambertProj->getNumberOfTuples(), 0);

  float vec[3];
  size_t bin = 0;
  for(int i = 0; i < totalPoints; i++)
  {
    vec[0] = m_VectorData[3*i+0];
    vec[1] = m_VectorData[3*i+1];
    vec[2] = m_VectorData[3*i+2];
    bin = determineSquareCoordsandBin(vec);
    if (dType.compare("int8_t") == 0)
    {
      addToLambert<int8_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("uint8_t") == 0)
    {
      addToLambert<uint8_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("int16_t") == 0)
    {
      addToLambert<int16_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("uint16_t") == 0)
    {
      addToLambert<uint16_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("int32_t") == 0)
    {
      addToLambert<int32_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("uint32_t") == 0)
    {
      addToLambert<uint32_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("int64_t") == 0)
    {
      addToLambert<int64_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("uint64_t") == 0)
    {
      addToLambert<uint64_t>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("float") == 0)
    {
      addToLambert<float>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("double") == 0)
    {
      addToLambert<double>(correlatedData, bin, i, m_LambertProjection);
    }
    else if (dType.compare("bool") == 0)
    {
      addToLambert<bool>(correlatedData, bin, i, m_LambertProjection);
    }
    counts[bin]++;
  }

  for(int i=0;i<m_LambertProj->getNumberOfTuples();i++)
  {
    for(int j=0;j<numComps;j++)
    {
      if(counts[i] == 0) m_LambertProjection[(numComps*i)+j] = 0;
      else m_LambertProjection[(numComps*i)+j] /= double(counts[i]);
    }
  }

  writeLambertProjection(numComps);
  createSterographicProjections(numComps);
  writePFStats(numComps);

  notifyStatusMessage(getHumanLabel(), "Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::makeLambertProjection(size_t numComps)
{
  QVector<size_t> tDims(2, m_Dimension);
  QVector<size_t> cDims(1, numComps);
  m_LambertProj = DoubleArrayType::CreateArray(tDims, cDims, "ModifiedLambertProjection");
  m_LambertProj->initializeWithZeros();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CorrelateValuesWithVectorDirection::determineSquareCoordsandBin(float xyz[3])
{
  float sqCoord[2];
  float adjust = -1.0;
  if(xyz[2] < 0.0)
  {
    MatrixMath::Multiply3x1withConstant(xyz, -1);
  }
  if(xyz[0] == 0 && xyz[1] == 0)
  {
    sqCoord[0] = 0.0;
    sqCoord[1] = 0.0;
  }
  if(fabs(xyz[0]) >= fabs(xyz[1]))
  {
    sqCoord[0] = (xyz[0] / fabs(xyz[0]) ) * sqrt(2.0 * 1.0 * (1.0 + (xyz[2] * adjust) ) ) * DREAM3D::Constants::k_HalfOfSqrtPi;
    sqCoord[1] = (xyz[0] / fabs(xyz[0]) ) * sqrt(2.0 * 1.0 * (1.0 + (xyz[2] * adjust) ) ) * ((DREAM3D::Constants::k_2OverSqrtPi) * atan(xyz[1] / xyz[0]));
  }
  else
  {
    sqCoord[0] = (xyz[1] / fabs(xyz[1])) * sqrt(2.0 * 1.0 * (1.0 + (xyz[2] * adjust))) * ((DREAM3D::Constants::k_2OverSqrtPi) * atan(xyz[0] / xyz[1]));
    sqCoord[1] = (xyz[1] / fabs(xyz[1])) * sqrt(2.0 * 1.0 * (1.0 + (xyz[2] * adjust))) * (DREAM3D::Constants::k_HalfOfSqrtPi);
  }

  if (sqCoord[0] >= m_MaxCoord)
  {
    sqCoord[0] = (m_MaxCoord) - .0001;
  }
  if (sqCoord[1] >= m_MaxCoord)
  {
    sqCoord[1] = (m_MaxCoord) - .0001;
  }
  int x = (int)( (sqCoord[0] + m_MaxCoord) / m_StepSize);
  if (x >= m_Dimension)
  {
    x = m_Dimension - 1;
  }
  if (x < 0) { x = 0; }
  int y = (int)( (sqCoord[1] + m_MaxCoord) / m_StepSize);
  if (y >= m_Dimension)
  {
    y = m_Dimension - 1;
  }
  if (y < 0) { y = 0; }
  int index = y * m_Dimension + x;
  BOOST_ASSERT(index < m_Dimension * m_Dimension);
  return index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::determineXYZCoords(float sqCoords[2], float xyz[3])
{
  float adjust = -1.0;

  if(fabs(sqCoords[0]) >= fabs(sqCoords[1]))
  {
    xyz[0] = (2.0*sqCoords[0]/DREAM3D::Constants::k_Pi) * sqrt((DREAM3D::Constants::k_Pi-((sqCoords[0]*sqCoords[0])/(1.0*1.0)))) * cosf((sqCoords[1]*DREAM3D::Constants::k_Pi)/(4.0*sqCoords[0]));
    xyz[1] = (2.0*sqCoords[0]/DREAM3D::Constants::k_Pi) * sqrt((DREAM3D::Constants::k_Pi-((sqCoords[0]*sqCoords[0])/(1.0*1.0)))) * sinf((sqCoords[1]*DREAM3D::Constants::k_Pi)/(4.0*sqCoords[0]));
    xyz[2] = 1.0 - ((2.0*sqCoords[0]*sqCoords[0])/(DREAM3D::Constants::k_Pi*1.0));
  }
  else
  {
    xyz[0] = (2.0*sqCoords[1]/DREAM3D::Constants::k_Pi) * sqrt((DREAM3D::Constants::k_Pi-((sqCoords[1]*sqCoords[1])/(1.0*1.0)))) * sinf((sqCoords[0]*DREAM3D::Constants::k_Pi)/(4.0*sqCoords[1]));
    xyz[1] = (2.0*sqCoords[1]/DREAM3D::Constants::k_Pi) * sqrt((DREAM3D::Constants::k_Pi-((sqCoords[1]*sqCoords[1])/(1.0*1.0)))) * cosf((sqCoords[0]*DREAM3D::Constants::k_Pi)/(4.0*sqCoords[1]));
    xyz[2] = 1.0 - ((2.0*sqCoords[1]*sqCoords[1])/(DREAM3D::Constants::k_Pi*1.0));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::createSterographicProjections(size_t numComps)
{
  int xpoints = 200;
  int ypoints = 200;
  int zpoints = 1;

  int xpointshalf = xpoints / 2;
  int ypointshalf = ypoints / 2;

  float xres = 2.0/float(xpoints);
  float yres = 2.0/float(ypoints);
  float zres = (xres+yres)/2.0;
  float xtmp, ytmp;
  float xyz[3];

  QVector<size_t> tDims(2, 0);
  tDims[0] = xpoints;
  tDims[1] = ypoints;
  QVector<size_t> cDims(1, numComps);
  DoubleArrayType::Pointer stereoIntensity = DoubleArrayType::CreateArray(tDims, cDims, "StereoProjection");
  stereoIntensity->initializeWithZeros();
  double* intensity = stereoIntensity->getPointer(0);
  double* m_LambertProjection = m_LambertProj->getPointer(0);

  int sqIndex = 0;
  for (int64_t y = 0; y < ypoints; y++)
  {
    for (int64_t x = 0; x < xpoints; x++)
    {
      //get (x,y) for stereographic projection pixel
      xtmp = float(x - xpointshalf) * xres + (xres * 0.5);
      ytmp = float(y - ypointshalf) * yres + (yres * 0.5);
      int index = y * xpoints + x;
      if((xtmp * xtmp + ytmp * ytmp) <= 1.0)
      {
        //project xy from stereo projection to the unit spehere
        xyz[2] = -((xtmp * xtmp + ytmp * ytmp) - 1) / ((xtmp * xtmp + ytmp * ytmp) + 1);
        xyz[0] = xtmp * (1 + xyz[2]);
        xyz[1] = ytmp * (1 + xyz[2]);

        sqIndex = determineSquareCoordsandBin(xyz);

        for(size_t i = 0; i < numComps; i++)
        {
          intensity[(numComps*index)+i] = m_LambertProjection[(numComps*sqIndex)+i];
        }
      }
    }
  }

  FILE* f = NULL;
  QString m_OutputFile = "c:\\Users\\groebema\\Desktop\\Data\\NDE\\test.vtk";
  f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if(NULL == f)
  {

    QString ss = QObject::tr("Could not open GBCD viz file %1 for writing. Please check access permissions and the path to the output location exists").arg(m_OutputFile);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY");
  fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints + 1, ypoints + 1, zpoints + 1);

  // Write the Coords
  writeCoords(f, "X_COORDINATES", "float", xpoints + 1, (-float(xpoints)*xres / 2.0), xres);
  writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-float(ypoints)*yres / 2.0), yres);
  writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-float(zpoints)*zres / 2.0), zres);

  size_t total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", (int)total);

  for(int iter=0;iter<numComps;iter++)
  {
    QString name = "Intensity" + QString::number(iter);
    fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), "float");
    fprintf(f, "LOOKUP_TABLE default\n");

    float* gn = new float[total];
    float t;
    int count = 0;
    for (int64_t j = 0; j < (ypoints); j++)
    {
      for (int64_t i = 0; i < (xpoints); i++)
      {
        t = float(intensity[(numComps*((j * xpoints) + i))+iter]);
        DREAM3D::Endian::FromSystemToBig::convert(t);
        gn[count] = t;
        count++;
      }
    }
    int64_t totalWritten = fwrite(gn, sizeof(float), (total), f);
    delete[] gn;
    if (totalWritten != (total))
    {
      qDebug() << "Error Writing Binary VTK Data into file " << m_OutputFile ;
      fclose(f);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::writeLambertProjection(size_t numComps)
{
  int xpoints = m_Dimension;
  int ypoints = m_Dimension;
  int zpoints = 1;

  float xres = m_StepSize;
  float yres = m_StepSize;
  float zres = (xres+yres)/2.0;

  QVector<size_t> tDims(2, 0);
  tDims[0] = xpoints;
  tDims[1] = ypoints;
  QVector<size_t> cDims(1, numComps);
  DoubleArrayType::Pointer modLamIntensity = DoubleArrayType::CreateArray(tDims, cDims, "ModLamProjection");
  modLamIntensity->initializeWithZeros();
  double* intensity = modLamIntensity->getPointer(0);
  double* m_LambertProjection = m_LambertProj->getPointer(0);

  for (int64_t y = 0; y < ypoints; y++)
  {
    for (int64_t x = 0; x < xpoints; x++)
    {
      //get (x,y) for stereographic projection pixel
      int index = y * xpoints + x;

      for(size_t i = 0; i < numComps; i++)
      {
        intensity[(numComps*index)+i] = m_LambertProjection[(numComps*index)+i];
      }
    }
  }

  FILE* f = NULL;
  QString m_OutputFile = "c:\\Users\\groebema\\Desktop\\Data\\NDE\\test2.vtk";
  f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if(NULL == f)
  {

    QString ss = QObject::tr("Could not open GBCD viz file %1 for writing. Please check access permissions and the path to the output location exists").arg(m_OutputFile);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY");
  fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints + 1, ypoints + 1, zpoints + 1);

  // Write the Coords
  writeCoords(f, "X_COORDINATES", "float", xpoints + 1, (-float(xpoints)*xres / 2.0), xres);
  writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-float(ypoints)*yres / 2.0), yres);
  writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-float(zpoints)*zres / 2.0), zres);

  size_t total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", (int)total);

  for(int iter=0;iter<numComps;iter++)
  {
    QString name = "Intensity" + QString::number(iter);
    fprintf(f, "SCALARS %s %s 1\n", name.toLatin1().data(), "float");
    fprintf(f, "LOOKUP_TABLE default\n");

    float* gn = new float[total];
    float t;
    int count = 0;
    for (int64_t j = 0; j < (ypoints); j++)
    {
      for (int64_t i = 0; i < (xpoints); i++)
      {
        t = float(intensity[(numComps*((j * xpoints) + i))+iter]);
        DREAM3D::Endian::FromSystemToBig::convert(t);
        gn[count] = t;
        count++;
      }
    }
    int64_t totalWritten = fwrite(gn, sizeof(float), (total), f);
    delete[] gn;
    if (totalWritten != (total))
    {
      qDebug() << "Error Writing Binary VTK Data into file " << m_OutputFile ;
      fclose(f);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CorrelateValuesWithVectorDirection::writePFStats(size_t numComps)
{
  int xpoints = m_Dimension;
  int ypoints = m_Dimension;

  float xres = m_StepSize;
  float yres = m_StepSize;

  double* m_LambertProjection = m_LambertProj->getPointer(0);
  QVector<float> amps(18, 0);
  QVector<float> means(18, 0);
  QVector<float> zbinCnts(18, 0);
  QVector<float> pshfts(72, 0);
  QVector<float> angbinCnts(72, 0);

  float sqCoord[2];
  float xyz[3];
  float ang;
  int zbin, angbin;
  for (int64_t y = 0; y < ypoints; y++)
  {
    for (int64_t x = 0; x < xpoints; x++)
    {
      int index = y * xpoints + x;
      sqCoord[0] = float(x*xres) - (float(xpoints*xres) / 2.0) + float(xres/2.0);
      sqCoord[1] = float(y*yres) - (float(ypoints*yres) / 2.0) + float(yres/2.0);
      
      determineXYZCoords(sqCoord, xyz);

      zbin = int((asinf(xyz[2])*180.0/DREAM3D::Constants::k_Pi)/5.0);
      ang = atan2(xyz[1],xyz[0])*180.0/DREAM3D::Constants::k_Pi;
      if(ang < 0) ang += 360.0;
      angbin = int(ang/5.0);

      if(zbin >= 18) zbin = 17;
      if(angbin >= 72) zbin = 71;

      means[zbin] += m_LambertProjection[(numComps*index)+0];
      amps[zbin] += m_LambertProjection[(numComps*index)+0];
      zbinCnts[zbin]++;
      if(zbin == 0)
      {
        pshfts[angbin] += m_LambertProjection[(numComps*index)+0];
        angbinCnts[angbin]++;
      }
    }
  }

  FILE* f = NULL;
  QString m_OutputFile = "c:\\Users\\groebema\\Desktop\\Data\\NDE\\PFstats.txt";
  f = fopen(m_OutputFile.toLatin1().data(), "wb");
  if(NULL == f)
  {

    QString ss = QObject::tr("Could not open GBCD viz file %1 for writing. Please check access permissions and the path to the output location exists").arg(m_OutputFile);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  for(int iter=0;iter<72;iter++)
  {
    if(iter < 18) fprintf(f, "%d %f %d %f %d %f \n", iter*5, pshfts[iter]/angbinCnts[iter], iter*5, amps[iter]/zbinCnts[iter], iter*5, means[iter]/zbinCnts[iter]);
    else fprintf(f, "%d %f\n", iter*5, pshfts[iter]/angbinCnts[iter]);
  }
  fclose(f);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CorrelateValuesWithVectorDirection::newFilterInstance(bool copyFilterParameters)
{
  /*
  */
  CorrelateValuesWithVectorDirection::Pointer filter = CorrelateValuesWithVectorDirection::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}
