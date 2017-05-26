/**
 * @file SampleDatabase.cpp implements methods for the sample database
 */

#include <stdexcept>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "SampleDatabase.hpp"


void SampleDatabase::read(const QJsonObject& object, const QString& fileName)
{
  QFileInfo fileInfo(fileName);
  name = fileInfo.fileName();
  QJsonArray audioFileArray = object["audioFiles"].toArray();
  audioFiles.clear();
  for (int audioFileIndex = 0; audioFileIndex < audioFileArray.size(); audioFileIndex++)
  {
    QJsonObject whistleLabelObject = audioFileArray[audioFileIndex].toObject();
    AudioFile audioFile;
    audioFile.read(whistleLabelObject, fileInfo.absoluteDir());
    audioFiles.append(audioFile);
  }
  exists = true;
}

void SampleDatabase::write(QJsonObject& object) const
{
  QJsonArray audioFileArray;
  foreach (const AudioFile audioFile, audioFiles)
  {
    QJsonObject audioFileObject;
    audioFile.write(audioFileObject);
    audioFileArray.append(audioFileObject);
  }
  object["audioFiles"] = audioFileArray;
}

void SampleDatabase::readFromFile(const QString& fileName)
{
  QFile inFile(fileName);
  if (!inFile.open(QIODevice::ReadOnly))
  {
    throw std::runtime_error("Could not open sample database file for reading!");
  }
  QByteArray fileContent = inFile.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(fileContent);
  read(doc.object(), fileName);
}

void SampleDatabase::writeToFile(const QString& fileName)
{
  QFile outFile(fileName);
  if (!outFile.open(QIODevice::WriteOnly))
  {
    throw std::runtime_error("Could not open sample database file for writing!");
  }
  QJsonObject dbObject;
  write(dbObject);
  QJsonDocument doc(dbObject);
  outFile.write(doc.toJson());
}

void SampleDatabase::clear()
{
  exists = false;
  audioFiles.clear();
}
