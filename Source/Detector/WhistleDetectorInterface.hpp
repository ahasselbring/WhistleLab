/**
 * @file WhistleDetectorInterface.hpp declares an interface for whistle detectors
 */

#pragma once

#include <vector>

#include "SampleDatabase.hpp"


/**
 * @class WhistleDetectorInterface is an interface for single channel whistle detectors
 */
class WhistleDetectorInterface
{
public:
  /**
   * @brief ~WhistleDetectorInterface virtual destructor for polymorphism
   */
  virtual ~WhistleDetectorInterface() = default;
  /**
   * @brief getPreferredBufferSize returns the number of samples that the detector wants to get
   * @return the number of samples that the detector wants to get
   */
  virtual unsigned int getPreferredBufferSize() = 0;
  /**
   * @brief classify should determine whether a short audio snippet contains a whistle sound
   * @param samples a single channel vector of audio samples that may or may not contain a whistle sound
   * @param sampleRate the sample rate in samples per second
   * @return true iff the given audio sample contains a whistle
   */
  virtual bool classify(const std::vector<float>& samples, const unsigned int sampleRate) = 0;
  /**
   * @brief evaluateOnDatabase evaluates a detector on a given database
   * @param db the database on which the detector is evaluated
   */
  void evaluateOnDatabase(const SampleDatabase& db);
};
