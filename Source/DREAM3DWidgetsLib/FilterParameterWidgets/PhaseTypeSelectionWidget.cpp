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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "PhaseTypeSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtGui/QComboBox>
#include <QtGui/QLabel>

#include "DREAM3DLib/Common/PhaseType.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  PhaseTypesFilterParameter* p = dynamic_cast<PhaseTypesFilterParameter*>(parameter);
  Q_ASSERT_X(NULL != p, "PhaseTypeSelectionWidget can ONLY be used with PhaseTypesFilterParameter FilterParameters", __FILE__);

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::PhaseTypeSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
PhaseTypeSelectionWidget::~PhaseTypeSelectionWidget()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setupGui()
{
  if(getFilter() == NULL)
  {
    return;
  }

  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if (getFilterParameter() == NULL)
  {
    return;
  }
  QString units = getFilterParameter()->getUnits();
  if(units.isEmpty() == false)
  {
    label->setText(getFilterParameter()->getHumanLabel() + " (" + units + ")");
  }
  else
  {
    label->setText(getFilterParameter()->getHumanLabel() );
  }



  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);
  dataContainerList->clear();
  attributeMatrixList->clear();
  // Now let the gui send signals like normal
  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);

  populateComboBoxes();

  updatePhaseComboBoxes();

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::populateComboBoxes()
{
  //  std::cout << "void PhaseTypeSelectionWidget::populateComboBoxesWithSelection()" << std::endl;


  // Now get the DataContainerArray from the Filter instance
  // We are going to use this to get all the current DataContainers
  DataContainerArray::Pointer dca = getFilter()->getDataContainerArray();
  if(NULL == dca.get()) { return; }

  // Check to see if we have any DataContainers to actually populate drop downs with.
  if(dca->getDataContainers().size() == 0)
  {
    return;
  }
  // Cache the DataContainerArray Structure for our use during all the selections
  m_DcaProxy = DataContainerArrayProxy(dca.get());

  // Populate the DataContainerArray Combo Box with all the DataContainers
  QList<DataContainerProxy> dcList = m_DcaProxy.list;
  QListIterator<DataContainerProxy> iter(dcList);

  while(iter.hasNext() )
  {
    DataContainerProxy dc = iter.next();
    if(dataContainerList->findText(dc.name) == -1 )
    {
      dataContainerList->addItem(dc.name);
    }
  }

  // Grab what is currently selected
  QString curDcName = dataContainerList->currentText();
  QString curAmName = attributeMatrixList->currentText();

  // Get what is in the filter
  QVariant qvSelectedPath = getFilter()->property(PROPERTY_NAME_AS_CHAR);
  DataArrayPath selectedPath = qvSelectedPath.value<DataArrayPath>();

  QString filtDcName = selectedPath.getDataContainerName();
  QString filtAmName = selectedPath.getAttributeMatrixName();

  QString dcName;
  QString amName;

  // If EVERYTHING is empty, then try the default value
  if(filtDcName.isEmpty() && filtAmName.isEmpty()
      && curDcName.isEmpty() && curAmName.isEmpty() )
  {
    DataArrayPath daPath = getFilterParameter()->getDefaultValue().value<DataArrayPath>();
    dcName = daPath.getDataContainerName();
    amName = daPath.getAttributeMatrixName();
  }
  else
  {

    // Now to figure out which one of these to use. If this is the first time through then what we picked up from the
    // gui will be empty strings because nothing is there. If there is something in the filter then we should use that.
    // If there is something in both of them and they are NOT equal then we have a problem. Use the flag m_DidCausePreflight
    // to determine if the change from the GUI should over ride the filter or vice versa. there is a potential that in future
    // versions that something else is driving DREAM3D and pushing the changes to the filter and we need to reflect those
    // changes in the GUI, like a testing script?

    dcName = checkStringValues(curDcName, filtDcName);
    amName = checkStringValues(curAmName, filtAmName);
  }
  bool didBlock = false;

  if (!dataContainerList->signalsBlocked()) { didBlock = true; }
  dataContainerList->blockSignals(true);
  int dcIndex = dataContainerList->findText(dcName);
  if(dcIndex < 0 && dcName.isEmpty() == false)
  {
    dataContainerList->addItem(dcName);
  } // the string was not found so just set it to the first index
  else
  {
    if(dcIndex < 0) { dcIndex = 0; } // Just set it to the first DataContainer in the list
    dataContainerList->setCurrentIndex(dcIndex);
    populateAttributeMatrixList();
  }
  if(didBlock) { dataContainerList->blockSignals(false); didBlock = false; }


  if(!attributeMatrixList->signalsBlocked()) { didBlock = true; }
  attributeMatrixList->blockSignals(true);
  int amIndex = attributeMatrixList->findText(amName);
  if(amIndex < 0 && amName.isEmpty() == false) { attributeMatrixList->addItem(amName); } // The name of the attributeMatrix was not found so just set the first one
  else
  {
    if(amIndex < 0) { amIndex = 0; }
    attributeMatrixList->setCurrentIndex(amIndex);
  }
  if(didBlock) { attributeMatrixList->blockSignals(false); didBlock = false; }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString PhaseTypeSelectionWidget::checkStringValues(QString curDcName, QString filtDcName)
{
  if(curDcName.isEmpty() == true && filtDcName.isEmpty() == false)
  {return filtDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == true)
  {return curDcName;}
  else if(curDcName.isEmpty() == false && filtDcName.isEmpty() == false && m_DidCausePreflight == true)
  { return curDcName;}

  return filtDcName;

}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::selectDefaultPath()
{

  // set the default DataContainer
  if(dataContainerList->count() > 0)
  {
    dataContainerList->setCurrentIndex(0);
  }

  // Set the default AttributeArray
  getFilter()->blockSignals(true);
  // Select the first AttributeMatrix in the list
  if(attributeMatrixList->count() > 0)
  {
    attributeMatrixList->setCurrentIndex(0);
  }
  getFilter()->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::setSelectedPath(QString dcName, QString attrMatName, QString attrArrName)
{
  // Set the correct DataContainer
  int count = dataContainerList->count();
  for(int i = 0; i < count; i++)
  {
    if (dataContainerList->itemText(i).compare(dcName) == 0 )
    {
      dataContainerList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

  // Set the correct AttributeMatrix
  count = attributeMatrixList->count();
  for(int i = 0; i < count; i++)
  {
    if (attributeMatrixList->itemText(i).compare(attrMatName) == 0 )
    {
      attributeMatrixList->setCurrentIndex(i); // This will fire the currentItemChanged(...) signal
      break;
    }
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::on_dataContainerList_currentIndexChanged(int index)
{

  //  std::cout << "void PhaseTypeSelectionWidget::on_dataContainerList_currentIndexChanged(int index)" << std::endl;
  populateAttributeMatrixList();

  // Select the first AttributeMatrix in the list
  if(attributeMatrixList->count() > 0)
  {
    on_attributeMatrixList_currentIndexChanged(0);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::populateAttributeMatrixList()
{
  QString dcName = dataContainerList->currentText();

  // Clear the AttributeMatrix List
  attributeMatrixList->blockSignals(true);
  attributeMatrixList->clear();

  // Loop over the data containers until we find the proper data container
  QList<DataContainerProxy> containers = m_DcaProxy.list;
  QListIterator<DataContainerProxy> containerIter(containers);
  while(containerIter.hasNext())
  {
    DataContainerProxy dc = containerIter.next();

    if(dc.name.compare(dcName) == 0 )
    {
      // We found the proper Data Container, now populate the AttributeMatrix List
      QMap<QString, AttributeMatrixProxy> attrMats = dc.attributeMatricies;
      QMapIterator<QString, AttributeMatrixProxy> attrMatsIter(attrMats);
      while(attrMatsIter.hasNext() )
      {
        attrMatsIter.next();
        QString amName = attrMatsIter.key();
        attributeMatrixList->addItem(amName);
      }
    }
  }

  attributeMatrixList->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::on_attributeMatrixList_currentIndexChanged(int index)
{
  resetPhaseComboBoxes();
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::updatePhaseComboBoxes()
{
  bool ok = false;
  // setup the list of choices for the widget
  PhaseTypesFilterParameter* phaseTypes = dynamic_cast<PhaseTypesFilterParameter*>(getFilterParameter());
  QString countProp = phaseTypes->getPhaseTypeCountProperty();
  int phaseCount = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);
  QString phaseDataProp = phaseTypes->getPhaseTypeDataProperty();

  UInt32Vector_t vectorWrapper = getFilter()->property(phaseDataProp.toLatin1().constData()).value<UInt32Vector_t>();
  QVector<quint32> dataFromFilter = vectorWrapper.d;
  if(phaseCount < 0 && dataFromFilter.size() < 10)   // there was an issue getting the phase Count from the Filter.
  {
    phaseCount = dataFromFilter.size(); // So lets just use the count from the actual phase data
  }

  // Get our list of predefined Phase Type Strings
  QVector<QString> phaseTypestrings;
  PhaseType::getPhaseTypeStrings(phaseTypestrings);
  // Get our list of predefined enumeration values
  QVector<unsigned int> phaseTypeEnums;
  PhaseType::getPhaseTypeEnums(phaseTypeEnums);

  phaseListWidget->clear();

  // We skip the first Ensemble as it is always a dummy
  //for (int i = 0; i < size; i++)
  for (int i = 1; i < phaseCount; i++)
  {
    QComboBox* cb = new QComboBox(NULL);
    for (qint32 s = 0; s < phaseTypestrings.size(); ++s)
    {
      cb->addItem((phaseTypestrings[s]), phaseTypeEnums[s]);
      cb->setItemData(static_cast<int>(s), phaseTypeEnums[s], Qt::UserRole);
    }

    QListWidgetItem* item = new QListWidgetItem(phaseListWidget);
    phaseListWidget->addItem(item);
    phaseListWidget->setItemWidget(item, cb);

    if (i < dataFromFilter.size())
    {
      cb->setCurrentIndex(dataFromFilter[i]);
      //qDebug() << "  Phase Data[" << i << "] = " << dataFromFilter[i];
    }
    connect(cb, SIGNAL(currentIndexChanged(int)),
            this, SLOT(phaseTypeComboBoxChanged(int)) );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::phaseTypeComboBoxChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::resetPhaseComboBoxes()
{
  int count = phaseListWidget->count();

  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    if (cb) { cb->setCurrentIndex(-1); }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    return;
  }
  dataContainerList->blockSignals(true);
  attributeMatrixList->blockSignals(true);

  // Reset all the combo box widgets to have the default selection of the first index in the list
  populateComboBoxes();
  //updatePhaseComboBoxes();

  dataContainerList->blockSignals(false);
  attributeMatrixList->blockSignals(false);


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::afterPreflight()
{
  updatePhaseComboBoxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void PhaseTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = phaseListWidget->count();
//  {
//    std::cout << "PhaseTypeSelectionWidget::filterNeedsInputParameters Count = " << count << std::endl;
//  }

  PhaseTypesFilterParameter* p = dynamic_cast<PhaseTypesFilterParameter*>(getFilterParameter());
  QVariant var;

  // QVector<uint32_t> phaseTypes(count, DREAM3D::PhaseType::UnknownPhaseType);
  QVector<uint32_t> phaseTypes(count + 1, DREAM3D::PhaseType::UnknownPhaseType);
  bool ok = false;
  phaseTypes[0] = DREAM3D::PhaseType::UnknownPhaseType;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = qobject_cast<QComboBox*>(phaseListWidget->itemWidget(phaseListWidget->item(i)));
    unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    //phaseTypes[i+1] = sType;
    phaseTypes[i + 1] = sType;
  }

  UInt32Vector_t data;
  data.d = phaseTypes;
  var.setValue(data);
  ok = false;

  const char* p1 = p->getPhaseTypeDataProperty().toLatin1().constData();
  // Set the value into the Filter
  ok = filter->setProperty(p1, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }


  const char* p2 = p->getAttributeMatrixPathProperty().toLatin1().constData();

  DataArrayPath path(dataContainerList->currentText(), attributeMatrixList->currentText(), "");

  var.setValue(path);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(p2, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }


}
