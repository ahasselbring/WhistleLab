#include <iostream>

#include "WhistleDetectorInterface.hpp"


void WhistleDetectorInterface::evaluateOnDatabase(const SampleDatabase& db)
{
  AudioSample as = db.getSample(getPreferredBufferSize());
  std::cout << "Predicted label: " << classify(as.samples, as.sampleRate) << '\n';
  std::cout << "Actual label: " << as.label << '\n';
}
