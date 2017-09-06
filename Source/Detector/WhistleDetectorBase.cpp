/**
 * @file WhistleDetectorBase.cpp implements methods shared among detectors
 */

#include <iostream>

#include "WhistleDetectorBase.hpp"


void WhistleDetectorBase::evaluateOnDatabase(const SampleDatabase& db, EvaluationResults* results)
{
  std::cout << "\n\nStart evaluation!\n\n";
  for (const auto& file : db.audioFiles)
  {
    EvaluationHandle eh(file);
    evaluate(eh);
    if (results == nullptr)
    {
      continue;
    }
    std::vector<unsigned int> labelHits(file.channels[0].whistleLabels.size(), 0);
    unsigned int lastFP = 0;
    for (auto pos : eh.detections)
    {
      bool hit = false;
      for (int i = 0; i < file.channels[0].whistleLabels.size(); i++)
      {
        auto& wl = file.channels[0].whistleLabels[i];
        if (static_cast<unsigned int>(wl.start) < pos && pos < static_cast<unsigned int>(wl.end))
        {
          labelHits[i]++;
          hit = true;
          break;
        }
      }
      if (!hit && file.channels[0].completelyLabeled)
      {
        // Only one false positive per second is counted as otherwise it would be unfair to detectors with small window sizes.
        if (lastFP == 0 || pos > lastFP + file.sampleRate)
        {
          results->falsePositives++;
          std::cout << "Whistle in " << file.path.toStdString() << " at " << pos << " (i.e. "
                    << (static_cast<float>(pos) / static_cast<float>(file.sampleRate)) << ") is a false positive!\n";
          lastFP = std::max(1U, pos);
        }
      }
    }
    for (int i = 0; i < file.channels[0].whistleLabels.size(); i++)
    {
      if (labelHits[i])
      {
        results->truePositives++;
      }
      std::cout << "Whistle in " << file.path.toStdString() << " at " << file.channels[0].whistleLabels[i].start
                << " (i.e. "
                << (static_cast<float>(file.channels[0].whistleLabels[i].start) / static_cast<float>(file.sampleRate))
                << ") has been " << (labelHits[i] ? "hit" : "missed") << "!\n";
    }
    results->positives += file.channels[0].whistleLabels.size();
  }
  if (results != nullptr)
  {
    std::cout << "False Detections: " << results->falsePositives << '\n';
    std::cout << "True Detections: " << results->truePositives << '/' << results->positives << '\n';
  }
}

void WhistleDetectorBase::trainOnDatabase(const SampleDatabase&)
{
  std::cerr << "The derived detector doesn't seem to support training!\n";
}
