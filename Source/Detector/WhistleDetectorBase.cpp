/**
 * @file WhistleDetectorBase.cpp implements methods shared among detectors
 */

#include <iostream>

#include "WhistleDetectorBase.hpp"


void WhistleDetectorBase::evaluateOnDatabase(const SampleDatabase& db)
{
  std::cout << "\n\nStart evaluation!\n\n";
  for (const auto& file : db.audioFiles)
  {
    EvaluationHandle eh(file);
    evaluate(eh);
    std::vector<unsigned int> labelHits(file.channels[0].whistleLabels.size(), 0);
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
        std::cout << "False detection in " << file.path.toStdString() << " at " << pos << " (i.e. "
                  << (static_cast<float>(pos) / static_cast<float>(file.sampleRate)) << ")!\n";
      }
    }
    for (int i = 0; i < file.channels[0].whistleLabels.size(); i++)
    {
      std::cout << "Whistle in " << file.path.toStdString() << " at " << file.channels[0].whistleLabels[i].start
                << " (i.e. "
                << (static_cast<float>(file.channels[0].whistleLabels[i].start) / static_cast<float>(file.sampleRate))
                << ") has been " << (labelHits[i] ? "hit" : "missed") << "!\n";
    }
  }
}

void WhistleDetectorBase::trainOnDatabase(const SampleDatabase&)
{
  std::cerr << "The derived detector doesn't seem to support training!\n";
}
