/**
 * @file LabelWidget.cpp implements methods for the label widget class
 */

#include "LabelWidget.hpp"


LabelWidget::LabelWidget(QWidget* parent)
  : QDockWidget(parent)
{
  setAllowedAreas(Qt::RightDockWidgetArea);
  setWindowTitle(tr("Labeling"));
}

void LabelWidget::updateSampleDatabase(const SampleDatabase sampleDatabase)
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
