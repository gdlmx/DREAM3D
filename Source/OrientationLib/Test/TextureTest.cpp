/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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


#include <iostream>
#include <vector>
#include <string>
#include "DREAM3DLib/Common/Texture.hpp"
#include "OrientationLib/OrientationOps/CubicOps.h"


/**
 * @brief These tests are just here to make sure the code compiles. The tests will
 * not actually work or would probably produced undefined results.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv)
{

  QVector<float> e1s;
  QVector<float> e2s;
  QVector<float> e3s;
  QVector<float> weights;
  QVector<float> sigmas;
  QVector<float> odf;

  size_t numEntries = e1s.size();
  odf.resize(CubicOps::k_OdfSize);
  Texture::CalculateCubicODFData(e1s.data(), e2s.data(), e3s.data(), weights.data(), sigmas.data(), true, odf.data(), numEntries);
  odf.resize(HexagonalOps::k_OdfSize);
  Texture::CalculateHexODFData(e1s.data(), e2s.data(), e3s.data(), weights.data(), sigmas.data(), true,
                               odf.data(), numEntries);


  odf.resize(OrthoRhombicOps::k_OdfSize);
  Texture::CalculateOrthoRhombicODFData(e1s.data(), e2s.data(), e3s.data(), weights.data(), sigmas.data(), true,
                                        odf.data(), numEntries);


  //int size = 1000;
  // Now generate the actual XY point data that gets plotted.
  // These are the output vectors
  QVector<float> angles;
  QVector<float> axes;
  QVector<float> mdf(CubicOps::k_MdfSize);

  Texture::CalculateMDFData<float, CubicOps>(angles.data(), axes.data(), weights.data(), odf.data(), mdf.data(), angles.size());

  return EXIT_SUCCESS;
}
