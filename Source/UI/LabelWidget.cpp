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
