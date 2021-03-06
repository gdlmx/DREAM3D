/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <vector>
#include <QtCore/QString>
#include <fstream>

#include <boost/shared_array.hpp>


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "OrientationLib/Math/OrientationMath.h"
#include "OrientationLib/OrientationOps/OrientationOps.h"
#include "OrientationLib/OrientationOps/CubicOps.h"
#include "OrientationLib/OrientationOps/HexagonalOps.h"
#include "OrientationLib/OrientationOps/OrthoRhombicOps.h"
#include "DREAM3DLib/Utilities/DREAM3DRandom.h"

/**
 * @class Texture Texture.h AIM/Common/Texture.h
 * @brief This class holds default data for Orientation Distribution Function
 * calculations that the DREAM3D package will perform.
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Micharl A. Groeber for US Air Force Research Laboratory
 * @date Feb 1, 2011
 * @version 1.0
 */

class Texture
{
  public:

    virtual ~Texture() {}

    /**
    * @brief This will calculate ODF data based on an array of weights that are
    * passed in and a Cubic Crystal Structure. The input data for the
    * euler angles is in Columnar fashion instead of row major format.
    * @param e1s Pointer to first Euler Angles
    * @param e2s Pointer to the second euler angles
    * @param e3s Pointer to the third euler angles
    * @param weights Pointer to the Array of weights values.
    * @param sigmas Pointer to the Array of sigma values.
    * @param normalize Should the ODF data be normalized by the totalWeight value
    * before returning.
    * @param odf (OUT) Pointer to the ODF array that is generated from this function. NOTE: The memory
    * for this MUST have already been allocated. Use CubicOps::k_OdfSize to allocate the proper amount
    * @param numEntries The number of entries of Angle/Weight/Sigmas
    */
    template<typename T>
    static void CalculateCubicODFData(T* e1s, T* e2s, T* e3s,
                                      T* weights, T* sigmas,
                                      bool normalize, T* odf, size_t numEntries)
    {
      DREAM3D_RANDOMNG_NEW()
      CubicOps ops;
      Int32ArrayType::Pointer textureBins = Int32ArrayType::CreateArray(numEntries, "TextureBins");
      int32_t* TextureBins = textureBins->getPointer(0);

      float addweight = 0;
      float totaladdweight = 0;
      float totalweight = float(3 * CubicOps::k_OdfSize);
      int bin, addbin;
      int bin1, bin2, bin3;
      int addbin1, addbin2, addbin3;
      float dist, fraction;
      float r1, r2, r3;

      for (size_t i = 0; i < numEntries; i++)
      {
        OrientationMath::EulertoRod(e1s[i], e2s[i], e3s[i], r1, r2, r3);
        ops.getODFFZRod(r1, r2, r3);
        bin = ops.getOdfBin(r1, r2, r3);
        TextureBins[i] = static_cast<int>(bin);
      }

      for (int i = 0; i < CubicOps::k_OdfSize; i++)
      {
        odf[i] = 0;
      }
      for (size_t i = 0; i < numEntries; i++)
      {
        bin = TextureBins[i];
        bin1 = bin % 18;
        bin2 = (bin / 18) % 18;
        bin3 = bin / (18 * 18);
        for (int j = -sigmas[i]; j <= sigmas[i]; j++)
        {
          int jsqrd = j * j;
          for (int k = -sigmas[i]; k <= sigmas[i]; k++)
          {
            int ksqrd = k * k;
            for (int l = -sigmas[i]; l <= sigmas[i]; l++)
            {
              int lsqrd = l * l;
              addbin1 = bin1 + int(j);
              addbin2 = bin2 + int(k);
              addbin3 = bin3 + int(l);
              //if(addbin1 < 0) addbin1 = addbin1 + 18;
              //if(addbin1 >= 18) addbin1 = addbin1 - 18;
              //if(addbin2 < 0) addbin2 = addbin2 + 18;
              //if(addbin2 >= 18) addbin2 = addbin2 - 18;
              //if(addbin3 < 0) addbin3 = addbin3 + 18;
              //if(addbin3 >= 18) addbin3 = addbin3 - 18;
              int good = 1;
              if(addbin1 < 0) { good = 0; }
              if(addbin1 >= 18) { good = 0; }
              if(addbin2 < 0) { good = 0; }
              if(addbin2 >= 18) { good = 0; }
              if(addbin3 < 0) { good = 0; }
              if(addbin3 >= 18) { good = 0; }
              addbin = (addbin3 * 18 * 18) + (addbin2 * 18) + (addbin1);
              dist = powf((jsqrd + ksqrd + lsqrd), 0.5);
              fraction = 1.0 - (double(dist / int(sigmas[i])) * double(dist / int(sigmas[i])));
              if(dist <= int(sigmas[i]) && good == 1)
              {
                addweight = (weights[i] * fraction);
                if(sigmas[i] == 0.0) { addweight = weights[i]; }
                odf[addbin] = odf[addbin] + addweight;
                totaladdweight = totaladdweight + addweight;
              }
            }
          }
        }
      }
      if(totaladdweight > totalweight)
      {
        float scale = (totaladdweight / totalweight);
        for (int i = 0; i < CubicOps::k_OdfSize; i++)
        {
          odf[i] = odf[i] / scale;
        }
      }
      float remainingweight = totalweight;
      for (int i = 0; i < CubicOps::k_OdfSize; i++)
      {
        remainingweight = remainingweight - odf[i];
      }
      float random1, random2, random3;
      float ea1, ea2, ea3;
      for (size_t i = 0; i < static_cast<size_t>(remainingweight); i++)
      {
        random1 = rg.genrand_res53();
        random2 = rg.genrand_res53();
        random3 = rg.genrand_res53();
        ea1 = 2.0 * DREAM3D::Constants::k_Pi * random1;
        ea2 = acos(2.0 * (random2 - 0.5));
        ea3 = 2.0 * DREAM3D::Constants::k_Pi * random3;
        OrientationMath::EulertoRod(ea1, ea2, ea3, r1, r2, r3);
        ops.getODFFZRod(r1, r2, r3);
        bin = ops.getOdfBin(r1, r2, r3);
        odf[bin]++;
      }
      if(normalize == true)
      {
        // Normalize the odf
        for (int i = 0; i < CubicOps::k_OdfSize; i++)
        {
          odf[i] = odf[i] / totalweight;
        }
      }
    }

    /**
    * @brief This will calculate ODF data based on an array of weights that are
    * passed in and a Hexagonal Crystal Structure. This is templated on the container
    * type that holds the data. Containers that adhere to the STL Vector API
    * should be usable. QVector falls into this category. The input data for the
    * euler angles is in Columnar fashion instead of row major format.
    * @param e1s The first euler angles
    * @param e2s The second euler angles
    * @param e3s The third euler angles
    * @param weights Array of weights values.
    * @param sigmas Array of sigma values.
    * @param normalize Should the ODF data be normalized by the totalWeight value
    * before returning.
    * @param odf (OUT) The ODF data that is generated from this function.
    * @param numEntries (OUT) The TotalWeight value that is also calculated
    */
    template<typename T>
    static void CalculateHexODFData(T* e1s, T* e2s, T* e3s, T* weights, T* sigmas, bool normalize, T* odf, size_t numEntries)
    {
      DREAM3D_RANDOMNG_NEW()
      Int32ArrayType::Pointer textureBins = Int32ArrayType::CreateArray(numEntries, "TextureBins");
      int32_t* TextureBins = textureBins->getPointer(0);
      float addweight = 0;
      float totaladdweight = 0;
      float totalweight = 3 * HexagonalOps::k_OdfSize;
      int bin, addbin;
      int bin1, bin2, bin3;
      int addbin1, addbin2, addbin3;
      float dist, fraction;
      //    float rmag, angle;
      float r1 = 0, r2 = 0, r3 = 0;
      HexagonalOps ops;
      for (size_t i = 0; i < numEntries; i++)
      {
        OrientationMath::EulertoRod(e1s[i], e2s[i], e3s[i], r1, r2, r3);
        ops.getODFFZRod(r1, r2, r3);
        bin = ops.getOdfBin(r1, r2, r3);
        TextureBins[i] = static_cast<int>(bin);
      }

      for (int i = 0; i < HexagonalOps::k_OdfSize; i++)
      {
        odf[i] = 0;
      }
      for (size_t i = 0; i < numEntries; i++)
      {
        bin = TextureBins[i];
        bin1 = bin % 36;
        bin2 = (bin / 36) % 36;
        bin3 = bin / (36 * 36);
        for (int j = -sigmas[i]; j <= sigmas[i]; j++)
        {
          int jsqrd = j * j;
          for (int k = -sigmas[i]; k <= sigmas[i]; k++)
          {
            int ksqrd = k * k;
            for (int l = -sigmas[i]; l <= sigmas[i]; l++)
            {
              int lsqrd = l * l;
              addbin1 = bin1 + int(j);
              addbin2 = bin2 + int(k);
              addbin3 = bin3 + int(l);
              //if(addbin1 < 0) addbin1 = addbin1 + 36;
              //if(addbin1 >= 36) addbin1 = addbin1 - 36;
              //if(addbin2 < 0) addbin2 = addbin2 + 36;
              //if(addbin2 >= 36) addbin2 = addbin2 - 36;
              //if(addbin3 < 0) addbin3 = addbin3 + 12;
              //if(addbin3 >= 12) addbin3 = addbin3 - 12;
              int good = 1;
              if(addbin1 < 0) { good = 0; }
              if(addbin1 >= 36) { good = 0; }
              if(addbin2 < 0) { good = 0; }
              if(addbin2 >= 36) { good = 0; }
              if(addbin3 < 0) { good = 0; }
              if(addbin3 >= 12) { good = 0; }
              addbin = (addbin3 * 36 * 36) + (addbin2 * 36) + (addbin1);
              dist = powf((jsqrd + ksqrd + lsqrd), 0.5);
              fraction = 1.0 - (double(dist / int(sigmas[i])) * double(dist / int(sigmas[i])));
              if(dist <= int(sigmas[i]) && good == 1)
              {
                addweight = (weights[i] * fraction);
                if(sigmas[i] == 0.0) { addweight = weights[i]; }
                odf[addbin] = odf[addbin] + addweight;
                totaladdweight = totaladdweight + addweight;
              }
            }
          }
        }
      }
      if(totaladdweight > totalweight)
      {
        float scale = (totaladdweight / totalweight);
        for (int i = 0; i < HexagonalOps::k_OdfSize; i++)
        {
          odf[i] = odf[i] / scale;
        }
      }
      float remainingweight = totalweight;
      for (int i = 0; i < HexagonalOps::k_OdfSize; i++)
      {
        remainingweight = remainingweight - odf[i];
      }
      float random1, random2, random3;
      float ea1, ea2, ea3;
      for (size_t i = 0; i < static_cast<size_t>(remainingweight); i++)
      {
        random1 = rg.genrand_res53();
        random2 = rg.genrand_res53();
        random3 = rg.genrand_res53();
        ea1 = 2.0 * DREAM3D::Constants::k_Pi * random1;
        ea2 = acos(2.0 * (random2 - 0.5));
        ea3 = 2.0 * DREAM3D::Constants::k_Pi * random3;
        OrientationMath::EulertoRod(ea1, ea2, ea3, r1, r2, r3);
        ops.getODFFZRod(r1, r2, r3);
        bin = ops.getOdfBin(r1, r2, r3);
        odf[bin]++;
      }
      if(normalize == true)
      {
        // Normalize the odf
        for (int i = 0; i < HexagonalOps::k_OdfSize; i++)
        {
          odf[i] = odf[i] / totalweight;
        }
      }
    }

    /**
    * @brief This will calculate ODF data based on an array of weights that are
    * passed in and a OrthoRhombic Crystal Structure. This is templated on the container
    * type that holds the data. Containers that adhere to the STL Vector API
    * should be usable. QVector falls into this category. The input data for the
    * euler angles is in Columnar fashion instead of row major format.
    * @param e1s The first euler angles
    * @param e2s The second euler angles
    * @param e3s The third euler angles
    * @param weights Array of weights values.
    * @param sigmas Array of sigma values.
    * @param normalize Should the ODF data be normalized by the totalWeight value
    * before returning.
    * @param odf (OUT) The ODF data that is generated from this function.
    * @param numEntries The TotalWeight value that is also calculated
    */
    template<typename T>
    static void CalculateOrthoRhombicODFData(T* e1s, T* e2s, T* e3s, T* weights, T* sigmas, bool normalize, T* odf, size_t numEntries)
    {
      DREAM3D_RANDOMNG_NEW()
      Int32ArrayType::Pointer textureBins = Int32ArrayType::CreateArray(numEntries, "TextureBins");
      int32_t* TextureBins = textureBins->getPointer(0);
      float addweight = 0;
      float totaladdweight = 0;
      float totalweight = 3 * OrthoRhombicOps::k_OdfSize;
      int bin, addbin;
      int bin1, bin2, bin3;
      int addbin1, addbin2, addbin3;
      float dist, fraction;
      float r1 = 0, r2 = 0, r3 = 0;
      OrthoRhombicOps ops;
      for (size_t i = 0; i < numEntries; i++)
      {
        OrientationMath::EulertoRod(e1s[i], e2s[i], e3s[i], r1, r2, r3);
        ops.getODFFZRod(r1, r2, r3);
        bin = ops.getOdfBin(r1, r2, r3);
        TextureBins[i] = static_cast<int>(bin);
      }

      for (int i = 0; i < OrthoRhombicOps::k_OdfSize; i++)
      {
        odf[i] = 0;
      }
      for (size_t i = 0; i < numEntries; i++)
      {
        bin = TextureBins[i];
        bin1 = bin % 36;
        bin2 = (bin / 36) % 36;
        bin3 = bin / (36 * 36);
        for (int j = -sigmas[i]; j <= sigmas[i]; j++)
        {
          int jsqrd = j * j;
          for (int k = -sigmas[i]; k <= sigmas[i]; k++)
          {
            int ksqrd = k * k;
            for (int l = -sigmas[i]; l <= sigmas[i]; l++)
            {
              int lsqrd = l * l;
              addbin1 = bin1 + int(j);
              addbin2 = bin2 + int(k);
              addbin3 = bin3 + int(l);
              //if (addbin1 < 0) addbin1 = addbin1 + 36;
              //if (addbin1 >= 36) addbin1 = addbin1 - 36;
              //if (addbin2 < 0) addbin2 = addbin2 + 36;
              //if (addbin2 >= 36) addbin2 = addbin2 - 36;
              //if (addbin3 < 0) addbin3 = addbin3 + 36;
              //if (addbin3 >= 36) addbin3 = addbin3 - 36;
              int good = 1;
              if (addbin1 < 0) { good = 0; }
              if (addbin1 >= 36) { good = 0; }
              if (addbin2 < 0) { good = 0; }
              if (addbin2 >= 36) { good = 0; }
              if (addbin3 < 0) { good = 0; }
              if (addbin3 >= 36) { good = 0; }
              addbin = (addbin3 * 36 * 36) + (addbin2 * 36) + (addbin1);
              dist = sqrtf(jsqrd + ksqrd + lsqrd);
              fraction = 1.0 - (float(dist / int(sigmas[i])) * float(dist / int(sigmas[i])));
              if (dist <= int(sigmas[i]) && good == 1)
              {
                addweight = (weights[i] * fraction);
                if(sigmas[i] == 0.0) { addweight = weights[i]; }
                odf[addbin] = odf[addbin] + addweight;
                totaladdweight = totaladdweight + addweight;
              }
            }
          }
        }
      }
      if(totaladdweight > totalweight)
      {
        float scale = (totaladdweight / totalweight);
        for (int i = 0; i < OrthoRhombicOps::k_OdfSize; i++)
        {
          odf[i] = odf[i] / scale;
        }
      }
      float remainingweight = totalweight;
      for (int i = 0; i < OrthoRhombicOps::k_OdfSize; i++)
      {
        remainingweight = remainingweight - odf[i];
      }
      float random1, random2, random3;
      float ea1, ea2, ea3;
      for (size_t i = 0; i < static_cast<size_t>(remainingweight); i++)
      {
        random1 = rg.genrand_res53();
        random2 = rg.genrand_res53();
        random3 = rg.genrand_res53();
        ea1 = 2.0 * DREAM3D::Constants::k_Pi * random1;
        ea2 = acos(2.0 * (random2 - 0.5));
        ea3 = 2.0 * DREAM3D::Constants::k_Pi * random3;
        OrientationMath::EulertoRod(ea1, ea2, ea3, r1, r2, r3);
        ops.getODFFZRod(r1, r2, r3);
        bin = ops.getOdfBin(r1, r2, r3);
        odf[bin]++;
      }
      if (normalize == true)
      {
        // Normalize the odf
        for (int i = 0; i < OrthoRhombicOps::k_OdfSize; i++)
        {
          odf[i] = odf[i] / totalweight;
        }
      }

    }

    /**
     * @brief CalculateMDFData Calculates MDF (Misorientation Distribution Function) data
     * @param angles The angles
     * @param axes The axes
     * @param weights The weights
     * @param odf The ODF which has been already computed and sized correctly in another function
     * @param mdf [output] The MDF array to store the data which has been preallocated already
     * @param numEntries The number of elemnts in teh Angles/Axes/Weights arrays which should all the be same size or at least
     * the value passed here is the minium size of all the arrays. The sizes of the ODF and MDF arrays are
     * determined by calling the getODFSize and getMDFSize functions of the parameterized OrientationOps class.
     */
    template<typename T, class OrientationOps>
    static void CalculateMDFData(T* angles, T* axes, T* weights, T* odf, T* mdf, size_t numEntries)
    {
      OrientationOps orientationOps;
      const int odfsize = orientationOps.getODFSize();
      const int mdfsize = orientationOps.getMDFSize();


      DREAM3D_RANDOMNG_NEW()

      int mbin;
      float w = 0;
      int choose1, choose2;
      float ea11, ea12, ea13;
      float ea21, ea22, ea23;
      QuatF q1;
      QuatF q2;
      float totaldensity;
      float r1, r2, r3;
      float n1, n2, n3;
      float random1, random2, density;

      for (int i = 0; i < mdfsize; i++)
      {
        mdf[i] = 0.0;
      }
      int remainingcount = 10000;
      int aSize = static_cast<int>(numEntries);
      for (int i = 0; i < aSize; i++)
      {
        OrientationMath::AxisAngletoRod(angles[i], axes[3 * i], axes[3 * i + 1], axes[3 * i + 2], r1, r2, r3);
        orientationOps.getMDFFZRod(r1, r2, r3);
        mbin = orientationOps.getMisoBin(r1, r2, r3);
        mdf[mbin] = -int((weights[i] / float(mdfsize)) * 10000.0);
        remainingcount = remainingcount + mdf[mbin];
      }

      for (int i = 0; i < remainingcount; i++)
      {
        random1 = rg.genrand_res53();
        random2 = rg.genrand_res53();
        choose1 = 0;
        choose2 = 0;

        totaldensity = 0;
        for (int j = 0; j < odfsize; j++)
        {
          density = odf[j];
          float d = totaldensity;
          totaldensity = totaldensity + density;
          if(random1 >= d && random1 < totaldensity) { choose1 = static_cast<int>(j); }
          if(random2 >= d && random2 < totaldensity) { choose2 = static_cast<int>(j); }
        }
        orientationOps.determineEulerAngles(choose1, ea11, ea12, ea13);
        OrientationMath::EulertoQuat(ea11, ea12, ea13, q1);
        orientationOps.determineEulerAngles(choose2, ea21, ea22, ea23);
        OrientationMath::EulertoQuat(ea21, ea22, ea23, q2);
        w = orientationOps.getMisoQuat(q1, q2, n1, n2, n3);
        OrientationMath::AxisAngletoRod(w, n1, n2, n3, r1, r2, r3);
        orientationOps.getMDFFZRod(r1, r2, r3);
        mbin = orientationOps.getMisoBin(r1, r2, r3);
        if(mdf[mbin] >= 0) { mdf[mbin]++; }
        if(mdf[mbin] < 0) { i = i - 1; }
      }
      for (int i = 0; i < mdfsize; i++)
      {
        if(mdf[i] < 0) { mdf[i] = -mdf[i]; }
        mdf[i] = mdf[i] / 10000.0;
      }

    }



  protected:
    Texture() {}

  private:
    Texture(const Texture&); // Copy Constructor Not Implemented
    void operator=(const Texture&); // Operator '=' Not Implemented

};

#endif /* TEXTURE_H_ */

