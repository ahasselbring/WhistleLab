/**
 * @file SampleDatabaseWidget.hpp declares the sample database widget class
 */

#pragma once

#include <QDockWidget>

#include "Engine/SampleDatabase.hpp"


class QTreeWidget;
class QWidget;

/**
 * @class SampleDatabaseWidget is a widget that views a sample database
 */
class SampleDatabaseWidget : public QDockWidget
{
  Q_OBJECT
public:
  /**
   * @brief SampleDatabaseWidget creates the sample database UI
   * @param parent the parent object
   */
  SampleDatabaseWidget(QWidget* parent = 0);
public slots:
  /**
   * @brief updateSampleDatabase updates the sample database that is viewed
   * @param sampleDatabase a copy of the new sample database
   */
  void updateSampleDatabase(const SampleDatabase sampleDatabase);
private:
  /// the tree that displays the sample database
  QTreeWidget* treeWidget = nullptr;
};
