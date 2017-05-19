/**
 * @file SampleDatabaseWidget.cpp implements methods for the sample database widget class
 */

#include <QTreeWidget>

#include "SampleDatabaseWidget.hpp"


SampleDatabaseWidget::SampleDatabaseWidget(QWidget* parent)
  : QDockWidget(parent)
{
  setAllowedAreas(Qt::LeftDockWidgetArea);
  setWindowTitle(tr("Sample Database"));

  treeWidget = new QTreeWidget(this);
  setWidget(treeWidget);
}
