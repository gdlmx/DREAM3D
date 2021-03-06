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

#ifndef _InsertAtoms_H_
#define _InsertAtoms_H_

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VolumeDataContainer.h"
#include "DREAM3DLib/DataContainers/SurfaceDataContainer.h"
#include "SyntheticBuilding/SyntheticBuildingConstants.h"
/**
 * @class InsertAtoms InsertAtoms.h DREAM3DLib/SyntheticBuilderFilters/InsertAtoms.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class InsertAtoms : public AbstractFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    DREAM3D_SHARED_POINTERS(InsertAtoms)
    DREAM3D_STATIC_NEW_MACRO(InsertAtoms)
    DREAM3D_TYPE_MACRO_SUPER(InsertAtoms, AbstractFilter)

    virtual ~InsertAtoms();

    DREAM3D_FILTER_PARAMETER(QString, VertexDataContainerName)
    Q_PROPERTY(QString VertexDataContainerName READ getVertexDataContainerName WRITE setVertexDataContainerName)
    DREAM3D_FILTER_PARAMETER(QString, VertexAttributeMatrixName)
    Q_PROPERTY(QString VertexAttributeMatrixName READ getVertexAttributeMatrixName WRITE setVertexAttributeMatrixName)

    DREAM3D_FILTER_PARAMETER(FloatVec3_t, LatticeConstants)
    Q_PROPERTY(FloatVec3_t LatticeConstants READ getLatticeConstants WRITE setLatticeConstants)
    DREAM3D_FILTER_PARAMETER(int, Basis)
    Q_PROPERTY(int Basis READ getBasis WRITE setBasis)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    DREAM3D_FILTER_PARAMETER(DataArrayPath, AvgQuatsArrayPath)
    Q_PROPERTY(DataArrayPath AvgQuatsArrayPath READ getAvgQuatsArrayPath WRITE setAvgQuatsArrayPath)

    DREAM3D_FILTER_PARAMETER(QString, AtomFeatureLabelsArrayName)
    Q_PROPERTY(QString AtomFeatureLabelsArrayName READ getAtomFeatureLabelsArrayName WRITE setAtomFeatureLabelsArrayName)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName()  { return DREAM3D::FilterSubGroups::ResolutionFilters; }
    virtual const QString getHumanLabel();
    virtual const QString getBrandingString() { return SyntheticBuilding::SyntheticBuildingPluginDisplayName + " Filter"; }

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
    virtual void updateVertexInstancePointers();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    InsertAtoms();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    virtual void assign_points(QVector<VertexArray::Pointer> points, QVector<BoolArrayType::Pointer> inFeature);

  private:
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)
    DEFINE_REQUIRED_DATAARRAY_VARIABLE(float, AvgQuats)
    DEFINE_CREATED_DATAARRAY_VARIABLE(int32_t, AtomFeatureLabels)

    InsertAtoms(const InsertAtoms&); // Copy Constructor Not Implemented
    void operator=(const InsertAtoms&); // Operator '=' Not Implemented
};

#endif /* InsertAtoms_H_ */



