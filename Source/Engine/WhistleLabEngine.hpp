/**
 * @file WhistleLabEngine.hpp declares the whistle lab engine class
 */

#pragma once

#include <QObject>

#include "Engine/SampleDatabase.hpp"


class QString;

/**
 * @class WhistleLabEngine is the main application class
 */
class WhistleLabEngine : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief WhistleLabEngine initializes members
   * @param parent the parent object
   */
  WhistleLabEngine(QObject* parent = 0);
signals:
  /**
   * @brief sampleDatabaseChanged signals that the sample database has changed
   * @param sampleDatabase a copy of the new sample database
   */
  void sampleDatabaseChanged(const SampleDatabase sampleDatabase);
public slots:
  /**
   * @brief evaluateDetector evaluates a detector on the currently opened database
   * @param name the name of the detector
   */
  void evaluateDetector(const QString& name);
  /**
   * @brief changeFile opens or closes the sample database
   * @param readFileName the name of the new database file or an empty string
   * @param writeFileName the name where the old database is written or an empty string
   */
  void changeFile(const QString& readFileName, const QString& writeFileName);
private:
  /// the open sample database
  SampleDatabase sampleDatabase;
};
