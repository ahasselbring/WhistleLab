/**
 * @file SampleDatabase.hpp declares the sample database class
 */

#pragma once

#include <QList>
#include <QMetaType>
#include <QString>

#include "Engine/AudioFile.hpp"


class QJsonObject;

/**
 * @class SampleDatabase is a database containing audio channels with whistle labels
 */
class SampleDatabase final
{
public:
  /**
   * @brief read deserializes the object
   * @param object the JSON object from which the object is deserialized
   * @param fileName the name of the file from which the database is deserialized
   */
  void read(const QJsonObject& object, const QString& fileName);
  /**
   * @brief write serializes the object
   * @param object the JSON object to which the serialization is written
   */
  void write(QJsonObject& object) const;
  /**
   * @brief readFromFile reads a sample database from a file
   * @param fileName the name of the file
   */
  void readFromFile(const QString& fileName);
  /**
   * @brief writeToFile writes a sample database to a file
   * @param fileName the name of the file
   */
  void writeToFile(const QString& fileName);
  /**
   * @brief clear clears the database and marks it as not existing
   */
  void clear();
  /// whether the sample database is existing
  bool exists = false;
  /// the name of the sample database
  QString name;
  /// a list of audio files in the database
  QList<AudioFile> audioFiles;
};

Q_DECLARE_METATYPE(SampleDatabase)
