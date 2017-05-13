/**
 * @file WhistleDetectorInterface.cpp implements methods shared among detectors
 */

#include <iostream>

#include "SampleProvider.hpp"

#include "WhistleDetectorInterface.hpp"


void WhistleDetectorInterface::evaluateOnDatabase(const SampleDatabase& db)
{
  const unsigned int maxNumberOfTests = 100000;
  SampleProvider sampleProvider(db);
  AudioSample as;
  unsigned int numberOfTests;
  unsigned int numberOfCorrectPredictions = 0;
  unsigned int offset = 0;
  for (numberOfTests = 0; numberOfTests < maxNumberOfTests; numberOfTests++)
  {
    as = sampleProvider.getSampleByOffset(offset, getPreferredBufferSize());
    if (as.sampleRate == 0)
    {
      break;
    }
    const float predictedLabel = classify(as.samples, as.sampleRate);
    if ((predictedLabel > 0.5f) == (as.label > 0.5f))
    {
      numberOfCorrectPredictions++;
    }
    offset += getPreferredBufferSize() / 2;
  }
  if (numberOfTests == 0)
  {
    std::cout << "The sample set seems to be very small!\n";
    return;
  }
  std::cout << "Accuracy: " << (static_cast<float>(numberOfCorrectPredictions) / static_cast<float>(numberOfTests)) << '\n';
}
