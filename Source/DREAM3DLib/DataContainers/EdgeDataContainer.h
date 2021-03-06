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

#ifndef _EDGEDATACONTAINER_H_
#define _EDGEDATACONTAINER_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif

//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

//-- Boost includes
#include <boost/shared_array.hpp>

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/Observable.h"

#include "DREAM3DLib/DataContainers/VertexDataContainer.h"

#include "DREAM3DLib/DataContainers/EdgeArray.hpp"


/**
 * @class EdgeDataContainer EdgeDataContainer.h DREAM3DLib/DataContainers/EdgeDataContainer.h
 * @brief This data container holds data the represents a SurfaceMesh
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Sep 28, 2012
 * @version 1.0
 */
class DREAM3DLib_EXPORT EdgeDataContainer : public VertexDataContainer
{
  public:
    DREAM3D_SHARED_POINTERS (EdgeDataContainer)
    DREAM3D_STATIC_NEW_MACRO (EdgeDataContainer)
    DREAM3D_TYPE_MACRO_SUPER(EdgeDataContainer, Observable)

    virtual ~EdgeDataContainer();

    /**
     * @brief New Creates a new data container with the give name
     * @param name The name of the data container
     * @return
     */
    static Pointer New(const QString& name)
    {
      Pointer sharedPtr(new EdgeDataContainer());
      sharedPtr->setName(name);
      return sharedPtr;
    }

    DREAM3D_INSTANCE_PROPERTY(EdgeArray::Pointer, Edges)

    /* ************ THESE ARE GOING TO GO AWAY I THINK. THEY ARE HERE TO GET THINGS TO COMPILE ******* */
    DREAM3D_INSTANCE_PROPERTY(Int32DynamicListArray::Pointer, MeshLinks)
    /* ************************************************************************************************* */

    virtual unsigned int getDCType() {return DREAM3D::DataContainerType::EdgeDataContainer;}

    virtual DataContainer::Pointer deepCopy();
    virtual int writeMeshToHDF5(hid_t dcGid, bool writeXdmf);
    virtual int writeEdgesToHDF5(hid_t dcGid);
    virtual int writeXdmf(QTextStream& out, QString hdfFileName);
    virtual int readEdges(hid_t dcGid, bool preflight);
    virtual int readMeshDataFromHDF5(hid_t dcGid, bool preflight);

  protected:
    EdgeDataContainer();

    virtual void writeXdmfMeshStructureHeader(QTextStream& out, QString hdfFileName);

  private:

    EdgeDataContainer(const EdgeDataContainer&);
    void operator =(const EdgeDataContainer&);

};

#endif /* EDGEDATACONTAINER_H_ */

