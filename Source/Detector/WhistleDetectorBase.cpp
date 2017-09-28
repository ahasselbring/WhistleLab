/**
 * @file WhistleDetectorBase.cpp implements methods shared among detectors
 */

#include <cassert>
#include <iostream>

#include "WhistleDetectorBase.hpp"


void WhistleDetectorBase::evaluateOnDatabase(const SampleDatabase& db, EvaluationResults* results)
{
  std::cout << "\n\nStart evaluation!\n\n";
  if (results != nullptr)
  {
    results->maximumDelay = 0.f;
    results->minimumDelay = std::numeric_limits<float>::max();
    results->averageDelay = 0.f;
    results->maximumExecutionTimePerTime = 0.f;
    results->minimumExecutionTimePerTime = std::numeric_limits<float>::max();
    results->averageExecutionTimePerTime = 0.f;
  }
  unsigned long numOfExecutions = 0;
  for (const auto& file : db.audioFiles)
  {
    EvaluationHandle eh(file);
    evaluate(eh);
    if (results == nullptr)
    {
      continue;
    }
    for (auto execTime : eh.executionTimes)
    {
      if (execTime > results->maximumExecutionTimePerTime)
      {
        results->maximumExecutionTimePerTime = execTime;
      }
      if (execTime < results->minimumExecutionTimePerTime)
      {
        results->minimumExecutionTimePerTime = execTime;
      }
      results->averageExecutionTimePerTime += execTime;
    }
    numOfExecutions += eh.executionTimes.size();
    assert(eh.detections.size() == eh.detectionPositions.size());
    std::vector<unsigned int> labelHits(file.channels[0].whistleLabels.size(), 0);
    std::vector<float> labelDelays(file.channels[0].whistleLabels.size(), std::numeric_limits<float>::max());
    unsigned int lastFP = 0;
    for (unsigned int j = 0; j < eh.detections.size(); j++)
    {
      const unsigned int pos = eh.detections[j];
      bool hit = false;
      for (int i = 0; i < file.channels[0].whistleLabels.size(); i++)
      {
        auto& wl = file.channels[0].whistleLabels[i];
        if (static_cast<unsigned int>(wl.start) < pos && pos < static_cast<unsigned int>(wl.end))
        {
          labelHits[i]++;
          // The detection cannot be made when the detector hasn't even read any of the data containing the whistle.
          assert(eh.detectionPositions[j] >= wl.start);
          labelDelays[i] = std::min(labelDelays[i], static_cast<float>(eh.detectionPositions[j] - wl.start) / static_cast<float>(file.sampleRate));
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
        if (labelDelays[i] > results->maximumDelay)
        {
          results->maximumDelay = labelDelays[i];
        }
        if (labelDelays[i] < results->minimumDelay)
        {
          results->minimumDelay = labelDelays[i];
        }
        results->averageDelay += labelDelays[i];
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
    if (results->truePositives != 0)
    {
      results->averageDelay /= static_cast<float>(results->truePositives);
    }
    if (numOfExecutions)
    {
      results->averageExecutionTimePerTime /= static_cast<float>(numOfExecutions);
    }
    std::cout << "False Detections: " << results->falsePositives << '\n';
    std::cout << "True Detections: " << results->truePositives << '/' << results->positives << '\n';
    std::cout << "Maximum Delay: " << results->maximumDelay << "s\n";
    std::cout << "Minimum Delay: " << results->minimumDelay << "s\n";
    std::cout << "Average Delay: " << results->averageDelay << "s\n";
    std::cout << "Maximum execution time ratio: " << results->maximumExecutionTimePerTime << '\n';
    std::cout << "Minimum execution time ratio: " << results->minimumExecutionTimePerTime << '\n';
    std::cout << "Average execution time ratio: " << results->averageExecutionTimePerTime << '\n';
  }
}

void WhistleDetectorBase::trainOnDatabase(const SampleDatabase&)
{
  std::cerr << "The derived detector doesn't seem to support training!\n";
}
