/**
 * @file EvaluationHandle.hpp declares the EvaluationHandle class
 */

#pragma once

#include <vector>

#include "Engine/AudioFile.hpp"


/**
 * @class EvaluationHandle delivers and collects data for the evaluation process
 */
class EvaluationHandle final
{
public:
  /**
   * @brief EvaluationHandle initializes members
   * @param af the audio file on which the detector is evaluated
   */
  explicit EvaluationHandle(const AudioFile& af);
  /**
   * @brief getSampleRate returns the sample rate of the audio file
   * @return the sample rate of the audio file
   */
  unsigned int getSampleRate() const;
  /**
   * @brief getNumberOfChannels returns the number of channels in the audio file
   * @return the number of channels in the audio file
   */
  unsigned int getNumberOfChannels() const;
  /**
   * @brief readSingleChannel reads samples from the first channel of the evaluated file
   * @param buf the buffer where the read samples are stored
   * @param length the number of samples that should be read
   * @return the number of actually read samples
   */
  unsigned int readSingleChannel(float* buf, unsigned int length);
  /**
   * @brief report reports a whistle detection
   * @param offset the offset of the detection to the current reading position
   */
  void report(int offset = 0);
  /**
   * @brief insideWhistle determines whether the reading position is inside a whistle
   * @param offset the offset of the query to the current reading position
   * @return whether the reading position (plus offset) is inside a whistle
   */
  int insideWhistle(int offset = 0) const;
private:
  /// the audio file on which the detector is evaluated
  const AudioFile& af;
  /// the current reading position
  unsigned int pos = 0;
  /// the vector that is filled with detections made by the detector
  std::vector<unsigned int> detections;
  friend class WhistleDetectorBase;
};
