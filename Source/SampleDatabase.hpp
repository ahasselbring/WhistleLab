/**
 * @file SampleDatabase.hpp declares datastructures for sample database handling
 */


#pragma once

#include <list>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>


/**
 * @struct WhistleLabel a range of samples where a whistle starts and ends
 */
struct WhistleLabel final
{
  /**
   * @brief WhistleLabel initializes members
   * @param start the first sample belonging to the whistle
   * @param end the first sample not belonging to the whistle anymore
   */
  WhistleLabel(const unsigned int start, const unsigned int end)
    : start(start)
    , end(end)
  {
  }
  /// the first sample belonging to the whistle
  unsigned int start;
  /// the first sample not belonging to the whistle anymore
  unsigned int end;
};

/**
 * @struct AudioChannel is a single channel of an audio file
 */
struct AudioChannel final
{
  /// the name of the corresponding file
  std::string name;
  /// the index of the corresponding channel
  unsigned int channel;
  /// the sample rate of the channel
  unsigned int sampleRate;
  /// the actual sequence of samples in the channel
  std::vector<float> samples;
  /// the set of labeled whistles in the channel
  std::vector<WhistleLabel> whistleLabels;
};

/**
 * @struct AudioFile represents a complete audio file
 */
struct AudioFile final
{
  /// the name of the corresponding file
  std::string name;
  /// the number of channels in the file
  unsigned int numberOfChannels;
  /// the sample rate of the file
  unsigned int sampleRate;
  /// the actual sequence of samples in the file
  std::vector<float> samples;
};

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
 * @class SampleDatabase is a database containing audio channels with whistle labels
 */
class SampleDatabase final
{
public:
  /**
   * @brief SampleDatabase loads a sample database from a file
   * @param path the path to the database file
   */
  SampleDatabase(const std::string& path);
  /**
   * @brief getSample gets a sample (a sequence of audio samples) from the database (hopefully 50% whistle and 50% non-whistle)
   * @param length the desired length (in samples) of the sample
   * @return an audio sample
   */
  AudioSample getSample(const unsigned int length) const;
private:
  /**
   * @brief getAudioChannel looks for an audio channel and loads it if it is not already loaded
   * @param name the name of the file
   * @param channel the index of the channel
   * @return a reference to the channel inside the database
   */
  AudioChannel& getAudioChannel(const std::string& name, const unsigned int channel);
  /**
   * @brief getAudioFile looks for an audio file and loads it if it is not already loaded
   * @param name the name of the file
   * @return a reference to the audio file
   */
  AudioFile& getAudioFile(const std::string& name);
  /**
   * @brief getLabel returns the ground truth label for a sample range in an audio channel
   * @param audioChannel the audio channel for which the label should be computed
   * @param start the first sample belonging to the range
   * @param end the first sample not belonging to the range anymore
   * @return the label (between 0 and 1)
   */
  float getLabel(const AudioChannel& audioChannel, const unsigned int start, const unsigned int end) const;
  /// the path in which the whistle database is located
  const boost::filesystem::path dirpath;
  /// a list of audio channels in the database
  std::list<AudioChannel> audioChannels;
  /// a list of audio files in the database (only valid during constructor)
  std::list<AudioFile> audioFiles;
  /// the total number of audio samples that is available in the database
  unsigned int totalAudioLength;
  /// the total number of whistle samples that is available in the database
  unsigned int totalWhistleLength;
};
