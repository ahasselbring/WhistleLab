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
  /// the index of the corresponding channel
  unsigned int channel;
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
  /// the channels of the file
  std::vector<AudioChannel> channels;
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
   * @brief getAudioFiles returns the list of audio files
   * @return the list of audio files
   */
  const std::list<AudioFile>& getAudioFiles() const;
private:
  /// the path in which the whistle database is located
  const boost::filesystem::path dirpath;
  /// a list of audio files in the database (only valid during constructor)
  std::list<AudioFile> audioFiles;
  /// the total number of audio samples that is available in the database
  unsigned int totalAudioLength;
  /// the total number of whistle samples that is available in the database
  unsigned int totalWhistleLength;
};
