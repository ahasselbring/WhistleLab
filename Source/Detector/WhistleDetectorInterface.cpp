#include <iostream>

#include "WhistleDetectorInterface.hpp"


void WhistleDetectorInterface::evaluateOnDatabase(const SampleDatabase& db)
{
  std::vector<float> samples;
  float label;
  unsigned int sampleRate;
  db.getSample(getPreferredBufferSize(), samples, sampleRate, label);
  std::cout << "Predicted label: " << classify(samples, sampleRate) << '\n';
  std::cout << "Actual label: " << label << '\n';
}
