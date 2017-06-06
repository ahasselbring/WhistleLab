/**
 * @file LabelWidget.hpp declares the label widget class
 */

#pragma once

#include <QDockWidget>


class QWidget;

/**
 * @class LabelWidget is a widget that allows labeling of audio data
 */
class LabelWidget : public QDockWidget
{
  Q_OBJECT
public:
  /**
   * @brief LabelWidget creates the label UI
   * @param parent the parent object
   */
  LabelWidget(QWidget* parent = 0);
};


