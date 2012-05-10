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
#include "MicrostructureStatisticsWidget.h"

//-- Qt Includes
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QThread>
#include <QtCore/QFileInfoList>
#include <QtGui/QFileDialog>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

#include "MXA/Utilities/MXADir.h"

#include "QtSupport/DREAM3DQtMacros.h"
#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/QCheckboxDialog.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"
#include "DREAM3DLib/IOFilters/FieldDataCSVWriter.h"
#include "DREAM3DLib/IOFilters/DataContainerReader.h"
#include "DREAM3DLib/PrivateFilters/FindNeighbors.h"
#include "DREAM3DLib/PrivateFilters/FindBoundingBoxGrains.h"
#include "DREAM3DLib/PrivateFilters/FindSurfaceGrains.h"
#include "DREAM3DLib/StatisticsFilters/FindSizes.h"
#include "DREAM3DLib/StatisticsFilters/FindShapes.h"
#include "DREAM3DLib/StatisticsFilters/FindSchmids.h"
#include "DREAM3DLib/StatisticsFilters/FindNeighborhoods.h"
#include "DREAM3DLib/StatisticsFilters/FindDeformationStatistics.h"
#include "DREAM3DLib/StatisticsFilters/FindLocalMisorientationGradients.h"
#include "DREAM3DLib/StatisticsFilters/FindAvgOrientations.h"
#include "DREAM3DLib/StatisticsFilters/FindAxisODF.h"
#include "DREAM3DLib/StatisticsFilters/FindODF.h"
#include "DREAM3DLib/StatisticsFilters/FindMDF.h"
#include "DREAM3DLib/StatisticsFilters/FindEuclideanDistMap.h"

#include "MicrostructureStatisticsPlugin.h"

#define MAKE_OUTPUT_FILE_PATH(outpath, filename, outdir, prefix)\
    std::string outpath = outdir + MXADir::Separator + prefix + filename;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsWidget::MicrostructureStatisticsWidget(QWidget *parent) :
  DREAM3DPluginFrame(parent), m_FilterPipeline(NULL), m_WorkerThread(NULL),
#if defined(Q_WS_WIN)
      m_OpenDialogLastDirectory("C:\\")
#else
      m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  setupGui();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MicrostructureStatisticsWidget::~MicrostructureStatisticsWidget()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::readSettings(QSettings &prefs)
{
  QString val;

  bool ok = false;
  double d;

  prefs.beginGroup("MicrostructureStatistics");
  READ_FILEPATH_SETTING(prefs, m_, InputFile, "");
  on_m_InputFile_textChanged(QString(""));

  READ_FILEPATH_SETTING(prefs, m_, OutputDir, "");
  READ_STRING_SETTING(prefs, m_, OutputFilePrefix, "MicrostructureStatistics_")

  READ_SETTING(prefs, m_, BinStepSize, ok, d, 1.0 , Double);

  READ_CHECKBOX_SETTING(prefs, m_, H5StatisticsFile, true)
  READ_CHECKBOX_SETTING(prefs, m_, GrainDataFile, true)
  READ_BOOL_SETTING(prefs, m_, WriteGrainSize, true);
  READ_BOOL_SETTING(prefs, m_, WriteGrainShapes, true);
  READ_BOOL_SETTING(prefs, m_, WriteNumNeighbors, true);
  READ_BOOL_SETTING(prefs, m_, WriteAverageOrientations, true);

  READ_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile, true);
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true);
  READ_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, true)
  READ_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, true)
  READ_BOOL_SETTING(prefs, m_, WriteKernelMisorientationsScalars, true)
  READ_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, true)
  READ_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, true)

  prefs.endGroup();
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::writeSettings(QSettings &prefs)
{
  prefs.beginGroup("MicrostructureStatistics");
  WRITE_STRING_SETTING(prefs, m_, OutputDir)
  WRITE_STRING_SETTING(prefs, m_, OutputFilePrefix)
  WRITE_STRING_SETTING(prefs, m_, InputFile)

  WRITE_SETTING(prefs, m_, BinStepSize);

  WRITE_CHECKBOX_SETTING(prefs, m_, H5StatisticsFile)
  WRITE_CHECKBOX_SETTING(prefs, m_, GrainDataFile)
  WRITE_BOOL_SETTING(prefs, m_, WriteGrainSize, m_WriteGrainSize);
  WRITE_BOOL_SETTING(prefs, m_, WriteGrainShapes, m_WriteGrainShapes);
  WRITE_BOOL_SETTING(prefs, m_, WriteNumNeighbors, m_WriteNumNeighbors);
  WRITE_BOOL_SETTING(prefs, m_, WriteAverageOrientations, m_WriteAverageOrientations);

  WRITE_CHECKBOX_SETTING(prefs, m_, VisualizationVizFile)
  WRITE_BOOL_SETTING(prefs, m_, WriteBinaryVTKFile, m_WriteBinaryVTKFile);
  WRITE_BOOL_SETTING(prefs, m_, WriteSurfaceVoxelScalars, m_WriteSurfaceVoxelScalars)
  WRITE_BOOL_SETTING(prefs, m_, WritePhaseIdScalars, m_WritePhaseIdScalars)
  WRITE_BOOL_SETTING(prefs, m_, WriteKernelMisorientationsScalars, m_WriteKernelMisorientationsScalars)
  WRITE_BOOL_SETTING(prefs, m_, WriteIPFColorScalars, m_WriteIPFColorScalars)

  prefs.endGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setWidgetListEnabled(bool b)
{
  foreach (QWidget* w, m_WidgetList)
    {
      w->setEnabled(b);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select H5Voxel File"), m_OpenDialogLastDirectory, tr("H5Voxel File (*.h5voxel)"));
  if (true == file.isEmpty())
  {
    return;
  }
  QFileInfo fi (file);
  m_InputFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_InputFile->setText(p);
  on_m_InputFile_textChanged(m_InputFile->text() );
  m_InputFile->blockSignals(false);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_InputFile_textChanged(const QString &text)
{
  if (verifyPathExists(m_InputFile->text(), m_InputFile) == true)
  {
    QFileInfo fi(m_InputFile->text());
    QString outPath = fi.absolutePath() + QDir::separator() + fi.baseName() + "_MicroStats";
    outPath = QDir::toNativeSeparators(outPath);
    m_OutputDir->setText(outPath);
    checkIOFiles();
  }

  QFileInfo fi(m_InputFile->text());
  if (fi.exists() && fi.isFile())
  {
    // Set the output file Prefix based on the name of the input file
    m_OutputFilePrefix->setText(fi.baseName() + QString("_"));

    // Load up the voxel data
    DataContainerReader::Pointer h5Reader = DataContainerReader::New();
//    H5VoxelReader::Pointer h5Reader = H5VoxelReader::New();
    h5Reader->setInputFile(m_InputFile->text().toStdString());
    int64_t dims[3];
    float spacing[3];
    float origin[3];
    int err = h5Reader->getSizeResolutionOrigin(dims, spacing, origin);
    if (err >= 0)
    {
      // These are the values from the data file and are displayed to the user
      m_XDim->setText(QString::number(dims[0]));
      m_YDim->setText(QString::number(dims[1]));
      m_ZDim->setText(QString::number(dims[2]));
      m_XRes->setText(QString::number(spacing[0]));
      m_YRes->setText(QString::number(spacing[1]));
      m_ZRes->setText(QString::number(spacing[2]));
    }

  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::checkIOFiles()
{
  verifyPathExists(m_OutputDir->text(), m_OutputDir);

  CHECK_QCHECKBOX_OUTPUT_FILE_EXISTS(DREAM3D::Reconstruction, m_ , VisualizationVizFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::MicroStats, m_, GrainDataFile)
  CHECK_QLABEL_OUTPUT_FILE_EXISTS(DREAM3D::MicroStats, m_, H5StatisticsFile)
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setupGui()
{

  QR3DFileCompleter* com2 = new QR3DFileCompleter(this, true);
  m_OutputDir->setCompleter(com2);
  QObject::connect(com2, SIGNAL(activated(const QString &)), this, SLOT(on_m_OutputDir_textChanged(const QString &)));

  QR3DFileCompleter* com3 = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com3);
  QObject::connect(com3, SIGNAL(activated(const QString &)), this, SLOT(on_m_InputFile_textChanged(const QString &)));

  QString msg("All files will be over written that appear in the output directory.");

  QFileInfo fi(m_OutputDir->text() + QDir::separator() + DREAM3D::SyntheticBuilder::VisualizationVizFile.c_str());

  m_WidgetList << m_OutputDir << m_OutputDirBtn;
  m_WidgetList << m_OutputFilePrefix;
  m_WidgetList << m_BinStepSize << m_H5StatisticsFile << m_GrainDataFile << m_GrainFileOptionsBtn;
  m_WidgetList << m_VisualizationVizFile << m_VtkOptionsBtn;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_LoadSettingsBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Settings File"), m_OpenDialogLastDirectory, tr("Settings File (*.txt)"));
  if (true == file.isEmpty())
  {
    return;
  }
  QSettings prefs(file, QSettings::IniFormat, this);
  readSettings(prefs);
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir))
  {
    checkIOFiles();
  }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_SaveSettingsBtn_clicked()
{
  QString proposedFile = m_OpenDialogLastDirectory + QDir::separator() + "MicrostructureStatisticsSettings.txt";
  QString file = QFileDialog::getSaveFileName(this, tr("Save Grain Generator Settings"), proposedFile, tr("*.txt"));
  if (true == file.isEmpty())
  {
    return;
  }

  QSettings prefs(file, QSettings::IniFormat, this);
  writeSettings(prefs);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_OutputDirBtn_clicked()
{
  QString outputFile = this->m_OpenDialogLastDirectory + QDir::separator();
  outputFile = QFileDialog::getExistingDirectory(this, tr("Select Microstructure Statistics Output Directory"), outputFile);
  if (!outputFile.isNull())
  {
    this->m_OutputDir->setText(outputFile);
    if (verifyPathExists(outputFile, m_OutputDir) == true)
    {
      checkIOFiles();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_OutputDir_textChanged(const QString &text)
{
  if (verifyPathExists(m_OutputDir->text(), m_OutputDir))
  {
    checkIOFiles();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_GoBtn_clicked()
{


  if (m_GoBtn->text().compare("Cancel") == 0)
  {
    if (m_FilterPipeline != NULL)
    {
      //std::cout << "canceling from GUI...." << std::endl;
      emit cancelPipeline();
    }
    return;
  }

  if (false == sanityCheckOutputDirectory(m_OutputDir, QString("MicroStats")))
  {
    return;
  }
  SANITY_CHECK_INPUT(m_, OutputDir)
  if (m_WorkerThread != NULL)
  {
    m_WorkerThread->wait(); // Wait until the thread is complete
    delete m_WorkerThread; // Kill the thread
    m_WorkerThread = NULL;
  }
  m_WorkerThread = new QThread(); // Create a new Thread Resource

  if (NULL != m_FilterPipeline)
  {
    delete m_FilterPipeline;
    m_FilterPipeline = NULL;
  }
  // This method will create all the needed filters and push them into the
  // pipeline in the correct order
  setupPipeline();

  // instantiate a subclass of the FilterPipeline Wrapper which adds Qt Signal/Slots
  // to the FilterPipeline Class
  m_FilterPipeline->moveToThread(m_WorkerThread);

  /* Connect the signal 'started()' from the QThread to the 'run' slot of the
   * Reconstruction object. Since the Reconstruction object has been moved to another
   * thread of execution and the actual QThread lives in *this* thread then the
   * type of connection will be a Queued connection.
   */
  // When the thread starts its event loop, start the MicrostructureStatistics going
  connect(m_WorkerThread, SIGNAL(started()), m_FilterPipeline, SLOT(run()));

  // When the Reconstruction ends then tell the QThread to stop its event loop
  connect(m_FilterPipeline, SIGNAL(finished() ), m_WorkerThread, SLOT(quit()));

  // When the QThread finishes, tell this object that it has finished.
  connect(m_WorkerThread, SIGNAL(finished()), this, SLOT( pipelineComplete() ));

  // Send Progress from the Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (updateProgress(int)), this, SLOT(pipelineProgress(int) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (progressMessage(QString)), this, SLOT(addProgressMessage(QString) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (warningMessage(QString)), this, SLOT(addWarningMessage(QString) ));

  // Send progress messages from Reconstruction to this object for display
  connect(m_FilterPipeline, SIGNAL (errorMessage(QString)), this, SLOT(addErrorMessage(QString) ));

  // If the use clicks on the "Cancel" button send a message to the Reconstruction object
  // We need a Direct Connection so the
  connect(this, SIGNAL(cancelPipeline() ), m_FilterPipeline, SLOT (on_CancelWorker() ), Qt::DirectConnection);

  // Now preflight this pipeline to make sure we can actually run it
  int err = m_FilterPipeline->preflightPipeline();
  // If any error occured during the preflight a dialog has been presented to the
  // user so simply exit
  if(err < 0)
  {
    delete m_FilterPipeline;
    m_FilterPipeline = NULL;
    // Show a Dialog with the error from the Preflight
    return;
  }
  DataContainer::Pointer m = DataContainer::New();
  m_FilterPipeline->setDataContainer(m);


  setWidgetListEnabled(false);
  emit
  pipelineStarted();
  m_WorkerThread->start();
  m_GoBtn->setText("Cancel");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::setupPipeline()
{
  // Create a FilterPipeline Object to hold all the filters. Later on we will execute all the filters
  m_FilterPipeline = new QFilterPipeline(NULL);
  std::string outDir = QDir::toNativeSeparators(m_OutputDir->text()).toStdString();
  std::string prefix = m_OutputFilePrefix->text().toStdString();


  m_ComputeGrainSize = false;
  m_ComputeGrainShapes = false;
  m_ComputeNumNeighbors = false;
  m_ComputeAverageOrientations = false;
  if(m_H5StatisticsFile->isChecked() == true || m_WriteGrainSize == true) m_ComputeGrainSize = true;
  if(m_H5StatisticsFile->isChecked() == true || m_WriteGrainShapes == true) m_ComputeGrainShapes = true;
  if(m_H5StatisticsFile->isChecked() == true || m_WriteNumNeighbors == true) m_ComputeNumNeighbors = true;
  if(m_WriteAverageOrientations == true) m_ComputeAverageOrientations = true;
  if(m_GrainDataFile->isChecked() == false)
  {
    m_WriteGrainSize = false;
    m_WriteGrainShapes = false;
    m_WriteNumNeighbors = false;
    m_WriteAverageOrientations = false;
  }
  if(m_VisualizationVizFile->isChecked() == false)
  {
    m_WriteSurfaceVoxelScalars = false;
    m_WritePhaseIdScalars = false;
    m_WriteKernelMisorientationsScalars = false;
    m_WriteIPFColorScalars = false;
    m_WriteBinaryVTKFile = false;
  }


  MAKE_OUTPUT_FILE_PATH( reconDeformStatsFile, DREAM3D::MicroStats::DeformationStatsFile, outDir, prefix);
  MAKE_OUTPUT_FILE_PATH( reconDeformIPFFile, DREAM3D::MicroStats::IPFDeformVTKFile, outDir, prefix);
  MAKE_OUTPUT_FILE_PATH( reconVisFile, DREAM3D::Reconstruction::VisualizationVizFile, outDir, prefix);
  MAKE_OUTPUT_FILE_PATH( hdf5ResultsFile, DREAM3D::MicroStats::H5StatisticsFile, outDir, prefix)


  DataContainerReader::Pointer h5Reader = DataContainerReader::New();
  h5Reader->setInputFile(m_InputFile->text().toStdString());
  m_FilterPipeline->pushBack(h5Reader);

  FindSurfaceGrains::Pointer find_surfacegrains = FindSurfaceGrains::New();
  m_FilterPipeline->pushBack(find_surfacegrains);

  // Start Computing the statistics
  if(m_ComputeGrainSize == true)
  {
    FindSizes::Pointer find_sizes = FindSizes::New();
    m_FilterPipeline->pushBack(find_sizes);
  }

  if(m_ComputeGrainShapes == true)
  {
    FindShapes::Pointer find_shapes = FindShapes::New();
    m_FilterPipeline->pushBack(find_shapes);

    FindBoundingBoxGrains::Pointer find_boundingboxgrains = FindBoundingBoxGrains::New();
    m_FilterPipeline->pushBack(find_boundingboxgrains);
  }

  if(m_ComputeNumNeighbors == true)
  {
    FindNeighbors::Pointer find_neighbors = FindNeighbors::New();
    m_FilterPipeline->pushBack(find_neighbors);

    FindNeighborhoods::Pointer find_neighborhoods = FindNeighborhoods::New();
    m_FilterPipeline->pushBack(find_neighborhoods);
  }

  if(m_WriteAverageOrientations == true
      || m_H5StatisticsFile->isChecked() == true
      || m_WriteKernelMisorientationsScalars == true)
  {
    FindAvgOrientations::Pointer find_avgorientations = FindAvgOrientations::New();
    m_FilterPipeline->pushBack(find_avgorientations);
  }

  if(m_H5StatisticsFile->isChecked() == true)
  {
    // Create a new Writer for the Stats Data.
    FindAxisODF::Pointer find_axisodf = FindAxisODF::New();
    m_FilterPipeline->pushBack(find_axisodf);

    FindODF::Pointer find_odf = FindODF::New();
    m_FilterPipeline->pushBack(find_odf);

    FindMDF::Pointer find_mdf = FindMDF::New();
    m_FilterPipeline->pushBack(find_mdf);
  }

  if(m_WriteKernelMisorientationsScalars == true)
  {
    FindLocalMisorientationGradients::Pointer find_localmisorientationgradients = FindLocalMisorientationGradients::New();
    m_FilterPipeline->pushBack(find_localmisorientationgradients);
  }


  FindSchmids::Pointer find_schmids = FindSchmids::New();
  m_FilterPipeline->pushBack(find_schmids);

  FindEuclideanDistMap::Pointer find_euclideandistmap = FindEuclideanDistMap::New();
  m_FilterPipeline->pushBack(find_euclideandistmap);

  FindDeformationStatistics::Pointer find_deformationstatistics = FindDeformationStatistics::New();
  find_deformationstatistics->setDeformationStatisticsFile(reconDeformStatsFile);
  find_deformationstatistics->setVtkOutputFile(reconDeformIPFFile);
  m_FilterPipeline->pushBack(find_deformationstatistics);



  if(m_GrainDataFile->isChecked() == true)
  {
    MAKE_OUTPUT_FILE_PATH( FieldDataFile, DREAM3D::MicroStats::GrainDataFile, outDir, prefix);
    FieldDataCSVWriter::Pointer write_fielddata = FieldDataCSVWriter::New();
    write_fielddata->setFieldDataFile(FieldDataFile);
    m_FilterPipeline->pushBack(write_fielddata);
  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::pipelineComplete()
{
  // std::cout << "MicrostructureStatisticsWidget::grainGenerator_Finished()" << std::endl;
  m_GoBtn->setText("Go");
  setWidgetListEnabled(true);
  this->m_progressBar->setValue(0);
  emit
  pipelineEnded();
  checkIOFiles();
  m_FilterPipeline->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::pipelineProgress(int val)
{
  this->m_progressBar->setValue(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_OutputFilePrefix_textChanged(const QString &text)
{
  checkIOFiles();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_VtkOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Write Surface Voxel Scalars");
  options.push_back("Write Phase Ids Scalars");
  options.push_back("Write Kernel Misorientation Scalars");
  options.push_back("Write IPF Color Scalars");
  options.push_back("Write Binary VTK File");
  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("VTK Output Options"));

  d.setValue("Write Surface Voxel Scalars", m_WriteSurfaceVoxelScalars);
  d.setValue("Write Phase Ids Scalars", m_WritePhaseIdScalars);
  d.setValue("Write Kernel Misorientation Scalars", m_WriteKernelMisorientationsScalars);
  d.setValue("Write IPF Color Scalars", m_WriteIPFColorScalars);
  d.setValue("Write Binary VTK File", m_WriteBinaryVTKFile);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteSurfaceVoxelScalars = d.getValue("Write Surface Voxel Scalars");
    m_WritePhaseIdScalars = d.getValue("Write Phase Ids Scalars");
    m_WriteKernelMisorientationsScalars = d.getValue("Write Kernel Misorientation Scalars");
    m_WriteIPFColorScalars = d.getValue("Write IPF Color Scalars");
    m_WriteBinaryVTKFile = d.getValue("Write Binary VTK File");
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::on_m_GrainFileOptionsBtn_clicked()
{
  QVector<QString> options;
  options.push_back("Grain Size");
  options.push_back("Grain Shape");
  options.push_back("Number Neighbors");
  options.push_back("Average Orientations");

  QCheckboxDialog d(options, this);
  d.setWindowTitle(QString("Grain File Output Options"));

  d.setValue("Grain Size", m_WriteGrainSize);
  d.setValue("Grain Shape", m_WriteGrainShapes);
  d.setValue("Number Neighbors", m_WriteNumNeighbors);
  d.setValue("Average Orientations", m_WriteAverageOrientations);

  int ret = d.exec();
  if (ret == QDialog::Accepted)
  {
    m_WriteGrainSize = d.getValue("Grain Size");
    m_WriteGrainShapes = d.getValue("Grain Shape");
    m_WriteNumNeighbors = d.getValue("Number Neighbors");
    m_WriteAverageOrientations = d.getValue("Average Orientations");
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::addErrorMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::MicrostructureStatisticsDisplayName).append(" Error");
  displayDialogBox(title, message, QMessageBox::Critical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::addWarningMessage(QString message)
{
  QString title = QString::fromStdString(DREAM3D::UIPlugins::MicrostructureStatisticsDisplayName).append(" Warning");
  displayDialogBox(title, message, QMessageBox::Warning);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicrostructureStatisticsWidget::addProgressMessage(QString message)
{
  if (NULL != this->statusBar()) {
    this->statusBar()->showMessage(message);
  }
}