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
#include "StatsGenRDFWidget.h"

#if QWT_VERSION >= 0x060000
#include "backwards.h"
#endif

//-- C++ Includes
#include <iostream>

//-- Qt Includes
#include <QtGui/QAbstractItemDelegate>
#include <QtCore/QtConcurrentMap>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>


#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_abstract_scale_draw.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_canvas.h>



#include "DREAM3DLib/Math/RadialDistributionFunction.h"

#include "Applications/StatsGenerator/TableModels/SGMDFTableModel.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenRDFWidget::StatsGenRDFWidget(QWidget* parent) :
  QWidget(parent),
  m_RDFTableModel(NULL)
{
  this->setupUi(this);
  this->setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StatsGenRDFWidget::~StatsGenRDFWidget()
{
  if (NULL != m_RDFTableModel)
  {
    m_RDFTableModel->deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SGRDFTableModel* StatsGenRDFWidget::tableModel()
{
  return m_RDFTableModel;
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenerateTestData(QwtPlot* plotPtr)
{
  QwtPlot& plot = *plotPtr;
  plot.setCanvasBackground(QColor(Qt::white));
  plot.setTitle("Histogram");

  QwtPlotGrid* grid = new QwtPlotGrid;
  grid->enableXMin(true);
  grid->enableYMin(true);
  grid->setMajPen(QPen(Qt::black, 0, Qt::DotLine));
  grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
  grid->attach(&plot);

  QwtHistogramItem* histogram = new QwtHistogramItem();
  histogram->setColor(Qt::darkCyan);

  const int numValues = 20;

  QwtArray<QwtDoubleInterval> intervals(numValues);
  QwtArray<double> values(numValues);

  double pos = 0.0;
  for ( int i = 0; i < (int)intervals.size(); i++ )
  {
    const int width = 5 + rand() % 15;
    const int value = rand() % 100;

    intervals[i] = QwtDoubleInterval(pos, pos + double(width));
    values[i] = value;

    pos += width;
  }

  histogram->setData(QwtIntervalData(intervals, values));
  histogram->attach(&plot);

  plot.setAxisScale(QwtPlot::yLeft, 0.0, 100.0);
  plot.setAxisScale(QwtPlot::xBottom, 0.0, pos);
  plot.replot();

#if QT_VERSION < 0x040000
  a.setMainWidget(&plot);
#endif

  plot.resize(600, 400);
  plot.show();
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::setupGui()
{
  initQwtPlot("Radial Distribution Plot", "Freq", m_RDFPlot);
  m_RDFTableModel = new SGRDFTableModel;
  m_RDFTableModel->setInitialValues();
  m_RDFTableView->setModel(m_RDFTableModel);
  QAbstractItemDelegate* aid = m_RDFTableModel->getItemDelegate();
  m_RDFTableView->setItemDelegate(aid);
  m_PlotCurve = new QwtPlotCurve;
  //GenerateTestData(m_RDFPlot);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::initQwtPlot(QString xAxisName, QString yAxisName, QwtPlot* plot)
{
  plot->setAxisTitle(QwtPlot::xBottom, xAxisName);
  plot->setAxisTitle(QwtPlot::yLeft, yAxisName);
  plot->setCanvasBackground( Qt::black ); //Set the Background colour
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::on_generateRDFBtn_clicked()
{
  // Generate the ODF Data from the current values in the ODFTableModel
  bool ok = false;
  float minDist = minDistLE->text().toFloat(&ok);
  float maxDist = maxDistLE->text().toFloat(&ok);
  int numBins = numBinsLE->text().toInt(&ok);

  std::vector<float> boxDims(3);
  boxDims[0] = 100;
  boxDims[1] = 100;
  boxDims[2] = 100;
  std::vector<float> boxRes(3);
  boxRes[0] = 0.1f;
  boxRes[1] = 0.1f;
  boxRes[2] = 0.1f;

  // Generate the RDF Frequencies
  std::vector<float> rdfFrequencies = RadialDistributionFunction::GenerateRandomDistribution(minDist, maxDist, numBins, boxDims, boxRes);
  QVector<float> qFreq = QVector<float>::fromStdVector(rdfFrequencies);

  // Update the Table model with the latest values
  m_RDFTableModel->setTableData(qFreq);

  // Update the Qwt plot with the correct values
  updateRDFPlot(qFreq);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::updateRDFPlot(QVector<float>& freqs)
{

  // These are the output vectors
  QwtArray<double> xD(static_cast<int>(freqs.size()));
  QwtArray<double> yD(static_cast<int>(freqs.size()));


  bool ok = false;
  float minDist = minDistLE->text().toFloat(&ok);
  float maxDist = maxDistLE->text().toFloat(&ok);

  const int numValues = freqs.size();
  float increment = (maxDist - minDist) / numValues;

  double pos = minDist;

  for (qint32 i = 0; i < numValues; ++i)
  {
    xD[i] = pos;
    yD[i] = static_cast<double>(freqs.at(i));
    pos = pos + increment;
  }

  // This will actually plot the XY data in the Qwt plot widget
  QwtPlotCurve* curve = m_PlotCurve;
#if QWT_VERSION >= 0x060000
  curve->setSamples(xD, yD);
#else
  curve->setData(xD, yD);
#endif
  curve->setStyle(QwtPlotCurve::Lines);
  //Use Antialiasing to improve plot render quality
  curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
  QPen pen;
  pen.setColor(Qt::white);
  pen.setWidth(2);
  curve->setPen(pen);//Set colour and thickness for drawing the curve
  curve->attach(m_RDFPlot);
  m_RDFPlot->replot();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StatsGenRDFWidget::extractStatsData(int index, StatsData* statsData, unsigned int phaseType)
{
  VectorOfFloatArray arrays;

  if(phaseType == DREAM3D::PhaseType::PrecipitatePhase)
  {
    PrecipitateStatsData* pp = PrecipitateStatsData::SafePointerDownCast(statsData);
    arrays = pp->getMDF_Weights();
  }

  if (arrays.size() > 0 )
  {
    QVector<float> angle(static_cast<int>(arrays[0]->getNumberOfTuples()));
    ::memcpy( &(angle.front()), arrays[0]->getVoidPointer(0), sizeof(float)*angle.size() );

    if (angle.size() > 0)
    {
      // Load the data into the table model
      m_RDFTableModel->setTableData(angle);
    }
  }
//  updateRDFPlot();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RdfData::Pointer StatsGenRDFWidget::getStatisticsData()
{
  bool ok = false;
  RdfData::Pointer rdf = RdfData::New();

  rdf->setMinDistance(minDistLE->text().toFloat(&ok));
  rdf->setMaxDistance(maxDistLE->text().toFloat(&ok));

  QVector<float> qRdfData = m_RDFTableModel->getData(SGRDFTableModel::Frequency);

  rdf->setFrequencies(qRdfData.toStdVector());

  return rdf;
}
