/**
 * @file WhistleDetectorInterface.cpp implements methods shared among detectors
 */

#include <iostream>

#include "SampleProvider.hpp"

#include "WhistleDetectorInterface.hpp"


void WhistleDetectorInterface::evaluateOnDatabase(const SampleDatabase& db)
{
  SampleProvider sampleProvider(db);
  AudioSample as;
  for (unsigned int i = 0; i < 100; i++)
  {
    as = sampleProvider.getSample(getPreferredBufferSize());
    std::cout << "Sample " << i << ":\n";
    std::cout << "Predicted label: " << classify(as.samples, as.sampleRate) << '\n';
    std::cout << "Actual label: " << as.label << '\n';
  }
}
