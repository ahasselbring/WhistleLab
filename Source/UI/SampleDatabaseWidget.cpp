/**
 * @file SampleDatabaseWidget.cpp implements methods for the sample database widget class
 */

#include <QHeaderView>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "SampleDatabaseWidget.hpp"


SampleDatabaseWidget::SampleDatabaseWidget(QWidget* parent)
  : QDockWidget(parent)
{
  setAllowedAreas(Qt::LeftDockWidgetArea);
  setWindowTitle(tr("Sample Database"));

  treeWidget = new QTreeWidget(this);
  treeWidget->header()->hide();

  setWidget(treeWidget);
}

void SampleDatabaseWidget::updateSampleDatabase(const SampleDatabase sampleDatabase)
{
  treeWidget->clear();
  if (!sampleDatabase.exists)
  {
    treeWidget->header()->hide();
    return;
  }
  treeWidget->setHeaderLabel(sampleDatabase.name);
  treeWidget->header()->show();
  QTreeWidgetItem* root = treeWidget->invisibleRootItem();
  for (auto& file : sampleDatabase.audioFiles)
  {
    QTreeWidgetItem* fileItem = new QTreeWidgetItem(root);
    fileItem->setText(0, file.path);
    for (auto& channel : file.channels)
    {
      QTreeWidgetItem* channelItem = new QTreeWidgetItem(fileItem);
      channelItem->setText(0, QString::number(channel.channel));
      for (auto& label : channel.whistleLabels)
      {
        QTreeWidgetItem* labelItem = new QTreeWidgetItem(channelItem);
        labelItem->setText(0, QString::number(static_cast<double>(label.start) / file.sampleRate)
          + " - " + QString::number(static_cast<double>(label.end) / file.sampleRate));
      }
    }
  }
}
