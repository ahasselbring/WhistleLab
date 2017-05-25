/**
 * @file WhistleLabEngine.cpp implements methods for the whistle lab engine class
 */

#include <QString>

#include "Detector/WhistleDetectorBase.hpp"
#include "Detector/WhistleDetectorFactoryBase.hpp"
#include "SampleDatabase.hpp"

#include "WhistleLabEngine.hpp"


WhistleLabEngine::WhistleLabEngine(QObject* parent)
  : QObject(parent)
{
}

void WhistleLabEngine::evaluateDetector(const QString& name)
{
  if (sampleDatabase == nullptr)
  {
    return;
  }

  auto detector = WhistleDetectorFactoryBase::make(name.toStdString());
  detector->evaluateOnDatabase(*sampleDatabase);
}

void WhistleLabEngine::changeFile(const QString& readFileName, const QString& writeFileName)
{
  if (sampleDatabase != nullptr && !writeFileName.isEmpty())
  {
    sampleDatabase->writeToFile(writeFileName);
  }
  delete sampleDatabase;
  sampleDatabase = nullptr;
  if (!readFileName.isEmpty())
  {
    sampleDatabase = new SampleDatabase;
    sampleDatabase->readFromFile(readFileName);
  }
}
