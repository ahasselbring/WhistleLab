/**
 * @file LabelWidget.cpp implements methods for the label widget class
 */

#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

#include "LabelWidget.hpp"


LabelWidget::LabelWidget(QWidget* parent)
  : QDockWidget(parent)
{
  setAllowedAreas(Qt::RightDockWidgetArea);
  setWindowTitle(tr("Labeling"));

  layoutWidget = new QWidget(this);

  playButton = new QPushButton("Play", layoutWidget);
  connect(playButton, &QPushButton::clicked, this, &LabelWidget::playClicked);

  pauseButton = new QPushButton("Pause", layoutWidget);
  connect(pauseButton, &QPushButton::clicked, this, &LabelWidget::pauseClicked);

  buttonLayout = new QHBoxLayout(layoutWidget);
  buttonLayout->addWidget(playButton);
  buttonLayout->addWidget(pauseButton);
  layoutWidget->setLayout(buttonLayout);
  setWidget(layoutWidget);
}

void LabelWidget::updateSampleDatabase(const SampleDatabase& sampleDatabase)
{
  this->sampleDatabase = sampleDatabase;
}

void LabelWidget::selectChannel(const QString& path, const unsigned int channel)
{
  if (!sampleDatabase.exists)
  {
    return;
  }

  emit channelSelectedForPlayback(path, channel);
}
