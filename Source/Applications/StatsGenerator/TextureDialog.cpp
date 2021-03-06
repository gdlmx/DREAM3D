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

#include "TextureDialog.h"

#include <QtGui/QDoubleValidator>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TextureDialog::TextureDialog(unsigned int xtal, QWidget* parent) :
  QDialog(parent),
  m_CrystalStructure(xtal)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TextureDialog::~TextureDialog()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::setupGui()
{
  if(Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure )
  {
    m_Presets = CubicTexturePresets::getTextures();
  }
  else if(Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure )
  {
    m_Presets = HexTexturePresets::getTextures();
  }

  // Clear the list view;
  presetListWidget->clear();

  // Now push the list into the List View
  for (TexturePreset::Container::iterator iter = m_Presets.begin(); iter != m_Presets.end(); ++iter)
  {
    presetListWidget->addItem(((*iter)->getName()));
  }

  {
    QDoubleValidator* validator = new QDoubleValidator(euler1);
    validator->setDecimals(4);
    euler1->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(euler2);
    validator->setDecimals(4);
    euler2->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(euler3);
    validator->setDecimals(4);
    euler3->setValidator(validator);
  }
  {
    QDoubleValidator* validator = new QDoubleValidator(weight);
    validator->setDecimals(4);
    weight->setValidator(validator);
  }
  {
    sigma->setRange(1.0, 1.0);
    if (Ebsd::CrystalStructure::Cubic_High == m_CrystalStructure  )
    {
      sigma->setRange(1.0, 18.0);
    }
    else if (Ebsd::CrystalStructure::Hexagonal_High == m_CrystalStructure )
    {
      sigma->setRange(1.0, 36.0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::on_presetListWidget_itemDoubleClicked ( QListWidgetItem* item )
{
  int index = presetListWidget->currentRow();
  TexturePreset::Pointer t = m_Presets[index];
  euler1Preset->setText(QString::number(t->getEuler1()));
  euler2Preset->setText(QString::number(t->getEuler2()));
  euler3Preset->setText(QString::number(t->getEuler3()));

  euler1->setText(QString::number(t->getEuler1()));
  euler2->setText(QString::number(t->getEuler2()));
  euler3->setText(QString::number(t->getEuler3()));

  weight->setText("1.0");
  sigma->setValue(1.0);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::on_presetListWidget_itemSelectionChanged()
{
  int index = presetListWidget->currentRow();
  TexturePreset::Pointer t = m_Presets[index];
  euler1Preset->setText(QString::number(t->getEuler1()));
  euler2Preset->setText(QString::number(t->getEuler2()));
  euler3Preset->setText(QString::number(t->getEuler3()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TextureDialog::getODFEntry(float& e1, float& e2, float& e3, float& w, float& s)
{
  bool ok = false;
  e1 = euler1->text().toFloat(&ok);
  e2 = euler2->text().toFloat(&ok);
  e3 = euler3->text().toFloat(&ok);
  w = weight->text().toFloat(&ok);
  s = sigma->value();
}


