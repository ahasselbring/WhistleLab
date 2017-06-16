/**
 * @file WhistleDetectorBase.cpp implements methods shared among detectors
 */

#include <iostream>

#include "SampleProvider.hpp"

#include "WhistleDetectorBase.hpp"


void WhistleDetectorBase::evaluateOnDatabase(const SampleDatabase& db)
{
  const unsigned int maxNumberOfTests = 100000;
  SampleProvider sampleProvider(db);
  AudioSample as;
  unsigned int numberOfTests;
  unsigned int numberOfPositives = 0, numberOfTruePositives = 0, numberOfTrueNegatives = 0;
  unsigned int offset = 0;
  for (numberOfTests = 0; numberOfTests < maxNumberOfTests; numberOfTests++)
  {
    as = sampleProvider.getSampleByOffset(offset, getPreferredBufferSize());
    if (as.sampleRate == 0)
    {
      break;
    }
    const float predictedLabel = classify(as.samples, as.sampleRate);
    const bool whistleDetected = predictedLabel > 0.5f;
    const bool whistleExpected = as.label > 0.5f;
    if (whistleExpected)
    {
      numberOfPositives++;
    }
    if (whistleDetected && whistleExpected)
    {
      numberOfTruePositives++;
    }
    else if (!whistleDetected && !whistleExpected)
    {
      numberOfTrueNegatives++;
    }
    offset += getPreferredBufferSize() / 2;
  }
  if (numberOfTests == 0)
  {
    std::cout << "The sample set seems to be very small!\n";
    return;
  }
  const unsigned int numberOfNegatives = numberOfTests - numberOfPositives;
  const unsigned int numberOfFalseNegatives = numberOfPositives - numberOfTruePositives;
  const unsigned int numberOfFalsePositives = numberOfNegatives - numberOfTrueNegatives;
  std::cout << "Sensitivity / Recall / Hit Rate / TPR: "
    << (static_cast<float>(numberOfTruePositives) / static_cast<float>(numberOfPositives))
    << '\n';
  std::cout << "Specificity / TNR: "
    << (static_cast<float>(numberOfTrueNegatives) / static_cast<float>(numberOfNegatives))
    << '\n';
  std::cout << "Precision / PPV: "
    << (static_cast<float>(numberOfTruePositives) / static_cast<float>(numberOfTruePositives + numberOfFalsePositives))
    << '\n';
  std::cout << "NPV: "
    << (static_cast<float>(numberOfTrueNegatives) / static_cast<float>(numberOfTrueNegatives + numberOfFalseNegatives))
    << '\n';
  std::cout << "Accuracy: "
    << (static_cast<float>(numberOfTruePositives + numberOfTrueNegatives) / static_cast<float>(numberOfTests))
    << '\n';
  std::cout << "Balanced accuracy: "
    << (0.5f * (static_cast<float>(numberOfTruePositives) / static_cast<float>(numberOfPositives)
      + static_cast<float>(numberOfTrueNegatives) / static_cast<float>(numberOfNegatives)))
    << '\n';
}
