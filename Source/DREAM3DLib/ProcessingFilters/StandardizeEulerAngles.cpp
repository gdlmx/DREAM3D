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
#include "StandardizeEulerAngles.h"

#if DREAM3D_USE_PARALLEL_ALGORITHMS
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/partitioner.h>
#include <tbb/task_scheduler_init.h>
#endif


#include "DREAM3DLib/Common/DREAM3DMath.h"
#include "DREAM3DLib/Common/OrientationMath.h"

#include "DREAM3DLib/OrientationOps/CubicOps.h"
#include "DREAM3DLib/OrientationOps/HexagonalOps.h"
#include "DREAM3DLib/OrientationOps/OrthoRhombicOps.h"


class StandardizeEulerAnglesImpl
{
    float* m_CellEulerAngles;
    int* m_CellPhases;
    unsigned int* m_CrystalStructures;

  std::vector<OrientationMath*> m_OrientationOps;
  OrientationMath::Pointer m_CubicOps;
  OrientationMath::Pointer m_HexOps;
  OrientationMath::Pointer m_OrthoOps;
  public:
    StandardizeEulerAnglesImpl(float* eulers, int* phases, unsigned int* crystructs) :
      m_CellEulerAngles(eulers),
    m_CellPhases(phases),
    m_CrystalStructures(crystructs)
    {
    m_HexOps = HexagonalOps::New();
    m_OrientationOps.push_back(m_HexOps.get());
    m_CubicOps = CubicOps::New();
    m_OrientationOps.push_back(m_CubicOps.get());
    m_OrthoOps = OrthoRhombicOps::New();
    m_OrientationOps.push_back(m_OrthoOps.get());
  }
    virtual ~StandardizeEulerAnglesImpl(){}

    void convert(size_t start, size_t end) const
    {

    float ea1, ea2, ea3;
   // float w, n1, n2, n3;
    float q[5];
  //  float min = 1;
//	  std::string filename = "Test.txt";
//	  std::ofstream outFile;
//	  outFile.open(filename.c_str());
      for (size_t i = start; i < end; i++)
      {
    ea1 = m_CellEulerAngles[3*i];
    ea2 = m_CellEulerAngles[3*i+1];
    ea3 = m_CellEulerAngles[3*i+2];
    OrientationMath::eulertoQuat(q, ea1, ea2, ea3);
    m_OrientationOps[m_CrystalStructures[m_CellPhases[i]]]->getFZQuat(q);
    OrientationMath::QuattoEuler(q, ea1, ea2, ea3);
    m_CellEulerAngles[3*i] = ea1;
    m_CellEulerAngles[3*i+1] = ea2;
    m_CellEulerAngles[3*i+2] = ea3;
//		outFile << ea1 << " " << ea2 << " " << ea3 << std::endl;
    }
    }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
    void operator()(const tbb::blocked_range<size_t> &r) const
    {
      convert(r.begin(), r.end());
    }
#endif


};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardizeEulerAngles::StandardizeEulerAngles() :
AbstractFilter(),
m_CellEulerAnglesArrayName(DREAM3D::CellData::EulerAngles),
m_CellPhasesArrayName(DREAM3D::CellData::Phases),
m_CrystalStructuresArrayName(DREAM3D::EnsembleData::CrystalStructures),
m_CellEulerAngles(NULL),
m_CellPhases(NULL),
m_CrystalStructures(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StandardizeEulerAngles::~StandardizeEulerAngles()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardizeEulerAngles::setupFilterParameters()
{
  std::vector<FilterParameter::Pointer> parameters;

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardizeEulerAngles::writeFilterParameters(AbstractFilterParametersWriter* writer)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardizeEulerAngles::dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles)
{
  setErrorCondition(0);
  std::stringstream ss;
  VoxelDataContainer* m = getVoxelDataContainer();

  GET_PREREQ_DATA(m, DREAM3D, CellData, CellEulerAngles, ss, -301, float, FloatArrayType, voxels, 3)
  GET_PREREQ_DATA(m, DREAM3D, CellData, CellPhases, ss, -302, int32_t, Int32ArrayType,  voxels, 1)

  typedef DataArray<unsigned int> XTalStructArrayType;
  GET_PREREQ_DATA(m, DREAM3D, EnsembleData, CrystalStructures, ss, -304, unsigned int, XTalStructArrayType, ensembles, 1)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardizeEulerAngles::preflight()
{
  dataCheck(true, 1, 1, 1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StandardizeEulerAngles::execute()
{
  VoxelDataContainer* m = getVoxelDataContainer();
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);
  int64_t totalPoints = m->getTotalPoints();
  size_t numgrains = m->getNumFieldTuples();
  size_t numensembles = m->getNumEnsembleTuples();
  dataCheck(false, totalPoints, numgrains, numensembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

#if DREAM3D_USE_PARALLEL_ALGORITHMS
  tbb::parallel_for(tbb::blocked_range<size_t>(0, totalPoints),
                    StandardizeEulerAnglesImpl(m_CellEulerAngles, m_CellPhases, m_CrystalStructures), tbb::auto_partitioner());

#else
  StandardizeEulerAnglesImpl serial(m_CellEulerAngles, m_CellPhases, m_CrystalStructures);
  serial.convert(0, totalPoints);
#endif

 notifyStatusMessage("Complete");
}