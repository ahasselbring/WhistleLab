/**
 * @file HULKsDetector.hpp declares a detector class
 */

#pragma once

#include "WhistleDetectorInterface.hpp"


/**
 * @brief HULKsDetector is the whistle detector of the SPL team HULKs
 */
class HULKsDetector : public WhistleDetectorInterface
{
public:
  /**
   * @brief getPreferredBufferSize returns the number of samples that the detector wants to get
   * @return the number of samples that the detector wants to get
   */
  unsigned int getPreferredBufferSize() override;
  /**
   * @brief classify should determine whether a short audio snippet contains a whistle sound
   * @param samples a single channel vector of audio samples that may or may not contain a whistle sound
   * @param sampleRate the sample rate in samples per second
   * @return true iff the given audio sample contains a whistle
   */
  bool classify(const std::vector<float>& samples, const unsigned int sampleRate) override;
};
