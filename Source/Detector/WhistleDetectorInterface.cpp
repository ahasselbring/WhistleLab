/**
 * @file WhistleDetectorInterface.cpp implements methods shared among detectors
 */

#include <iostream>

#include "WhistleDetectorInterface.hpp"


void WhistleDetectorInterface::evaluateOnDatabase(const SampleDatabase& db)
{
  AudioSample as;
  for (unsigned int i = 0; i < 100; i++)
  {
    as = db.getSample(getPreferredBufferSize());
    std::cout << "Sample " << i << ":\n";
    std::cout << "Predicted label: " << classify(as.samples, as.sampleRate) << '\n';
    std::cout << "Actual label: " << as.label << '\n';
  }
}
