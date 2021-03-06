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

#ifndef _CropVolume_H_
#define _CropVolume_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"

#include "Sampling/SamplingConstants.h"
/**
 * @class CropVolume CropVolume.h DREAM3DLib/SyntheticBuilderFilters/CropVolume.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class CropVolume : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(CropVolume)
    DREAM3D_STATIC_NEW_MACRO(CropVolume)
    DREAM3D_TYPE_MACRO_SUPER(CropVolume, AbstractFilter)

    virtual ~CropVolume();
    DREAM3D_FILTER_PARAMETER(QString, NewDataContainerName)
    Q_PROPERTY(QString NewDataContainerName READ getNewDataContainerName WRITE setNewDataContainerName)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellAttributeMatrixPath READ getCellAttributeMatrixPath WRITE setCellAttributeMatrixPath)
    DREAM3D_FILTER_PARAMETER(DataArrayPath, CellFeatureAttributeMatrixPath)
    Q_PROPERTY(DataArrayPath CellFeatureAttributeMatrixPath READ getCellFeatureAttributeMatrixPath WRITE setCellFeatureAttributeMatrixPath)

    IntVec3_t getCurrentVolumeDataContainerDimensions();
    Q_PROPERTY(IntVec3_t CurrentVolumeDataContainerDimensions READ getCurrentVolumeDataContainerDimensions)
    FloatVec3_t getCurrentVolumeDataContainerResolutions();
    Q_PROPERTY(FloatVec3_t CurrentVolumeDataContainerResolutions READ getCurrentVolumeDataContainerResolutions)


    DREAM3D_FILTER_PARAMETER(int, XMin)
    Q_PROPERTY(int XMin READ getXMin WRITE setXMin)
    DREAM3D_FILTER_PARAMETER(int, YMin)
    Q_PROPERTY(int YMin READ getYMin WRITE setYMin)
    DREAM3D_FILTER_PARAMETER(int, ZMin)
    Q_PROPERTY(int ZMin READ getZMin WRITE setZMin)

    DREAM3D_FILTER_PARAMETER(int, XMax)
    Q_PROPERTY(int XMax READ getXMax WRITE setXMax)
    DREAM3D_FILTER_PARAMETER(int, YMax)
    Q_PROPERTY(int YMax READ getYMax WRITE setYMax)
    DREAM3D_FILTER_PARAMETER(int, ZMax)
    Q_PROPERTY(int ZMax READ getZMax WRITE setZMax)
    DREAM3D_FILTER_PARAMETER(bool, RenumberFeatures)
    Q_PROPERTY(bool RenumberFeatures READ getRenumberFeatures WRITE setRenumberFeatures)
    DREAM3D_FILTER_PARAMETER(bool, SaveAsNewDataContainer)
    Q_PROPERTY(bool SaveAsNewDataContainer READ getSaveAsNewDataContainer WRITE setSaveAsNewDataContainer)
    DREAM3D_FILTER_PARAMETER(bool, UpdateOrigin)
    Q_PROPERTY(bool UpdateOrigin READ getUpdateOrigin WRITE setUpdateOrigin)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName()  { return DREAM3D::FilterSubGroups::CropCutFilters; }
    virtual const QString getHumanLabel();

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
       * @brief Reimplemented from @see AbstractFilter class
       */
    virtual void execute();
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    CropVolume();


  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    void dataCheck();

    CropVolume(const CropVolume&); // Copy Constructor Not Implemented
    void operator=(const CropVolume&); // Operator '=' Not Implemented
};

#endif /* CROPVOLUME_H_ */








