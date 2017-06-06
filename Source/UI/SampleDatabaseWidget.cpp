/**
 * @file SampleDatabaseWidget.cpp implements methods for the sample database widget class
 */

#include <QHeaderView>
#include <QMenu>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "SampleDatabaseWidget.hpp"


SampleDatabaseWidget::SampleDatabaseWidget(QWidget* parent)
  : QDockWidget(parent)
{
  setAllowedAreas(Qt::LeftDockWidgetArea);
  setWindowTitle(tr("Sample Database"));

  treeWidget = new QTreeWidget(this);
  connect(treeWidget, &QTreeWidget::customContextMenuRequested, this, &SampleDatabaseWidget::prepareMenu);
  treeWidget->setContextMenuPolicy(Qt::NoContextMenu);
  treeWidget->header()->hide();

  setWidget(treeWidget);
}

void SampleDatabaseWidget::updateSampleDatabase(const SampleDatabase sampleDatabase)
{
  treeWidget->clear();
  if (!sampleDatabase.exists)
  {
    treeWidget->setContextMenuPolicy(Qt::NoContextMenu);
    treeWidget->header()->hide();
    return;
  }
  treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
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

void SampleDatabaseWidget::prepareMenu(const QPoint& pos)
{
  QTreeWidgetItem* item = treeWidget->itemAt(pos);
  QMenu menu;
  if (item != nullptr)
  {
    if (item->parent() == nullptr)
    {
      QAction* removeFileAction = new QAction(tr("&Remove File"), this);
      menu.addAction(removeFileAction);
    }
    else if (item->parent()->parent() == nullptr)
    {
      QAction* openInLabelWidgetAction = new QAction(tr("&Open in Label Widget"), this);
      menu.addAction(openInLabelWidgetAction);
    }
    else
    {
      QAction* removeLabelAction = new QAction(tr("Remove &Label"), this);
      menu.addAction(removeLabelAction);

      QAction* setIntervalAction = new QAction(tr("&Set Interval"), this);
      menu.addAction(setIntervalAction);
    }
  }
  else
  {
    QAction* addFileAction = new QAction(tr("&Add File"), this);
    menu.addAction(addFileAction);
  }
  menu.exec(treeWidget->mapToGlobal(pos));
}
