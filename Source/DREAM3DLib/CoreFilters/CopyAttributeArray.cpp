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

#include "CopyAttributeArray.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyAttributeArray::CopyAttributeArray() :
  AbstractFilter(),
  m_SelectedArrayPath("", "", ""),
  m_NewArrayName("")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CopyAttributeArray::~CopyAttributeArray()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyAttributeArray::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Array to Copy", "SelectedArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getSelectedArrayPath(), false));
  parameters.push_back(FilterParameter::New("New Array Name", "NewArrayName", FilterParameterWidgetType::StringWidget, getNewArrayName(), false));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyAttributeArray::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setSelectedArrayPath( reader->readDataArrayPath("SelectedArrayPath", getSelectedArrayPath()) );
  setNewArrayName( reader->readString( "NewArrayName", getNewArrayName() ) );
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int CopyAttributeArray::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(SelectedArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(NewArrayName)
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyAttributeArray::dataCheck()
{
  setErrorCondition(0);

  if(m_NewArrayName.isEmpty() == true)
  {
    setErrorCondition(-11009);
    QString ss = QObject::tr("The New Attribute Array name can not be empty. Please set a value.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }

  if (m_SelectedArrayPath.isEmpty() == true)
  {
    setErrorCondition(-11010);
    QString ss = QObject::tr("The complete path to the Attribute Array can not be empty. Please set an appropriate path.");
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    return;
  }
  else
  {
    QString dcName = m_SelectedArrayPath.getDataContainerName();
    QString amName = m_SelectedArrayPath.getAttributeMatrixName();
    QString daName = m_SelectedArrayPath.getDataArrayName();

    DataContainer::Pointer dc = getDataContainerArray()->getDataContainer(dcName);
    if(NULL == dc.get())
    {
      setErrorCondition(-11007);
      QString ss = QObject::tr("The DataContainer '%1' was not found in the DataContainerArray").arg(dcName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    AttributeMatrix::Pointer attrMat = dc->getAttributeMatrix(amName);
    if(NULL == attrMat.get())
    {
      setErrorCondition(-11008);
      QString ss = QObject::tr("The AttributeMatrix '%1' was not found in the DataContainer '%2'").arg(amName).arg(dcName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    // We the AttributeMatrix, now lets try to get the AttributeArray object and copy it if it exists
    IDataArray::Pointer dataArray = attrMat->getAttributeArray(daName);
    if(NULL == dataArray.get() )
    {
      setErrorCondition(-11014);
      QString ss = QObject::tr("A DataArray with the name '%1' was not found in the AttributeMatrix").arg(daName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }
    IDataArray::Pointer p = attrMat->getAttributeArray(daName);
    IDataArray::Pointer pNew = p->deepCopy();
    int err = attrMat->addAttributeArray(m_NewArrayName, pNew);

    if(0 != err)
    {
      setErrorCondition(err);
      QString ss = QObject::tr("Attempt to copy AttributeArray '%1' to '%2' Failed.").arg(daName).arg(m_NewArrayName);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyAttributeArray::preflight()
{
  setInPreflight(true);
  emit preflightAboutToExecute();
  emit updateFilterParameters(this);
  dataCheck();
  emit preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyAttributeArray::execute()
{
  setErrorCondition(0);

  dataCheck(); // calling the dataCheck will copy the array, so nothing is required here

  if(getErrorCondition() < 0)
  {
    return;
  }

  notifyStatusMessage(getHumanLabel(), "Complete");
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer CopyAttributeArray::newFilterInstance(bool copyFilterParameters)
{
  /*
   * SelectedArrayPath
   * NewArrayName
   */
  CopyAttributeArray::Pointer filter = CopyAttributeArray::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyAttributeArray::getCompiledLibraryName()
{
  return Core::CoreBaseName;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyAttributeArray::getGroupName()
{
  return DREAM3D::FilterGroups::CoreFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyAttributeArray::getSubGroupName()
{
  return DREAM3D::FilterSubGroups::MemoryManagementFilters;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString CopyAttributeArray::getHumanLabel()
{
  return "Copy Attribute Array";
}

