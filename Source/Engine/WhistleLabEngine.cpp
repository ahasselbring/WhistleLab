/**
 * @file WhistleLabEngine.cpp implements methods for the whistle lab engine class
 */

#include <QString>

#include "Detector/WhistleDetectorBase.hpp"
#include "Detector/WhistleDetectorFactoryBase.hpp"

#include "WhistleLabEngine.hpp"


WhistleLabEngine::WhistleLabEngine(QObject* parent)
  : QObject(parent)
{
}

void WhistleLabEngine::evaluateDetector(const QString& name)
{
  if (!sampleDatabase.exists)
  {
    return;
  }

  auto detector = WhistleDetectorFactoryBase::make(name.toStdString());
  detector->evaluateOnDatabase(sampleDatabase);
}

void WhistleLabEngine::changeDatabase(const QString& readFileName, const QString& writeFileName)
{
  if (sampleDatabase.exists && !writeFileName.isEmpty())
  {
    sampleDatabase.writeToFile(writeFileName);
  }
  sampleDatabase.clear();
  if (!readFileName.isEmpty())
  {
    sampleDatabase.readFromFile(readFileName);
  }
  emit sampleDatabaseChanged(sampleDatabase);
}
