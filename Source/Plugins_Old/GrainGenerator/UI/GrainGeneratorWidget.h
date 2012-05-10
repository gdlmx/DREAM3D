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

#ifndef GRAINGENERATORWIDGET_H_
#define GRAINGENERATORWIDGET_H_

#include <vector>
#include <QtCore/QString>
#include <QtCore/QSettings>

#include "ui_GrainGeneratorWidget.h"

#include "QtSupport/DREAM3DPluginFrame.h"
#include "QFilterPipeline.h"

class QListWidgetItem;
class QComboBox;


/**
 * @class GrainGeneratorWidget GrainGeneratorWidget.h Plugins/GrainGenerator/UI/GrainGeneratorWidget.h
 * @brief This class represents the User Interface for the Synthetic Grain Generator module
 * of the DREAM3D program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 30, 2011
 * @version 1.0
 */
class GrainGeneratorWidget : public DREAM3DPluginFrame, private Ui::GrainGeneratorWidget
{
  Q_OBJECT

  public:
    GrainGeneratorWidget(QWidget *parent = 0);
    virtual ~GrainGeneratorWidget();

    /**
      * @brief Reads the Preferences from the users pref file
      */
    virtual void readSettings(QSettings &prefs);

     /**
      * @brief Writes the preferences to the users pref file
      */
    virtual void writeSettings(QSettings &prefs);

    /**
     * @brief Enables or Disables all the widgets in a list
     * @param b
     */
    virtual void setWidgetListEnabled(bool b);

    /**
     * @brief Initializes some of the GUI elements with selections or other GUI related items
     */
    virtual void setupGui();


    virtual void checkIOFiles();

    int estimate_numgrains(int xpoints, int ypoints, int zpoints,
                           float xres, float yres, float zres);
    void estimateNumGrainsSetup();

    void setupPipeline();

  protected slots:
    /* Grain Generator Slots*/
    void on_m_InputH5StatisticsFileBtn_clicked();
    void on_m_OutputDirBtn_clicked();
    void on_m_XPoints_valueChanged(int v);
    void on_m_YPoints_valueChanged(int v);
    void on_m_ZPoints_valueChanged(int v);
    void on_m_XResolution_valueChanged(double v);
    void on_m_YResolution_valueChanged(double v);
    void on_m_ZResolution_valueChanged(double v);

    //void on_m_AlreadyFormed_stateChanged(int);
    void on_m_GoBtn_clicked();
    void on_m_SaveSettingsBtn_clicked();
    void on_m_LoadSettingsBtn_clicked();

    void on_m_H5InputStatisticsFile_textChanged(const QString &text);
    void on_m_OutputDir_textChanged(const QString &text);
    void on_m_OutputFilePrefix_textChanged(const QString &text);
    void on_m_StructureFileBtn_clicked();

    void on_m_StructureFile_textChanged(const QString &text);

    void on_m_VtkOptionsBtn_clicked();
    void on_m_HDF5GrainFile_stateChanged(int state);

  private slots:

    // slots for our worker thread to communicate
    virtual void addErrorMessage(QString message);
    virtual void addWarningMessage(QString message);
    virtual void addProgressMessage(QString message);

    /* Reconstruction Thread communicates throught these methods */
    virtual void pipelineComplete();
    virtual void pipelineProgress(int value);


  private:
    QList<QWidget*>             m_WidgetList;
    QFilterPipeline*            m_FilterPipeline;
    QThread*                    m_WorkerThread;
    QList<QLabel*>              m_ShapeTypeLabels;
    QList<QComboBox*>           m_ShapeTypeCombos;
    DataContainer::Pointer      m_DataContainer;
    bool                        m_WriteSurfaceVoxelScalars;
    bool                        m_WritePhaseIdScalars;
    bool                        m_WriteIPFColorScalars;
    bool                        m_WriteBinaryVTKFile;

    QString m_OpenDialogLastDirectory;

    GrainGeneratorWidget(const GrainGeneratorWidget&); // Copy Constructor Not Implemented
    void operator=(const GrainGeneratorWidget&); // Operator '=' Not Implemented
};

#endif /* GRAINGENERATORWIDGET_H_ */