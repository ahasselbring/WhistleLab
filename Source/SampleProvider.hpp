/**
 * @file SampleProvider.hpp declares a class for sample selection
 */

#pragma once

#include "SampleDatabase.hpp"


/**
 * @struct AudioSample represents a audio sample (whistle or not) to be passed to a classifier
 */
struct AudioSample final
{
  /// the actual label of the whistle function
  float label;
  /// the sample rate of the sample
  unsigned int sampleRate;
  /// the actual sequence of audio samples
  std::vector<float> samples;
};

/**
 * @class SampleProvider is a class that can provide training/test samples with some required properties
 */
class SampleProvider
{
public:
  /**
   * @brief SampleProvider initializes members
   * @param sampleDatabase a reference to the sample database
   */
  SampleProvider(const SampleDatabase& sampleDatabase);
  /**
   * @brief getSample gets a sample (a sequence of audio samples) from the database (hopefully 50% whistle and 50% non-whistle)
   * @param length the desired length (in samples) of the sample
   * @return an audio sample
   */
  AudioSample getSample(const unsigned int length) const;
private:
  /**
   * @brief getLabel returns the ground truth label for a sample range in an audio channel
   * @param audioChannel the audio channel for which the label should be computed
   * @param start the first sample belonging to the range
   * @param end the first sample not belonging to the range anymore
   * @return the label (between 0 and 1)
   */
  float getLabel(const AudioChannel& audioChannel, const unsigned int start, const unsigned int end) const;
  /// a reference to the sample database
  const SampleDatabase& sampleDatabase;
};
