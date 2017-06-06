/**
 * @file WhistleLabEngine.hpp declares the whistle lab engine class
 */

#pragma once

#include <QAudio>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QByteArray>
#include <QObject>

#include "Engine/SampleDatabase.hpp"


class QAudioOutput;
class QString;

/**
 * @class WhistleLabEngine is the main application class
 */
class WhistleLabEngine : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief WhistleLabEngine initializes members
   * @param parent the parent object
   */
  WhistleLabEngine(QObject* parent = 0);
signals:
  /**
   * @brief sampleDatabaseChanged signals that the sample database has changed
   * @param sampleDatabase a copy of the new sample database
   */
  void sampleDatabaseChanged(const SampleDatabase sampleDatabase);
public slots:
  /**
   * @brief evaluateDetector evaluates a detector on the currently opened database
   * @param name the name of the detector
   */
  void evaluateDetector(const QString& name);
  /**
   * @brief changeDatabase opens or closes the sample database
   * @param readFileName the name of the new database file or an empty string
   * @param writeFileName the name where the old database is written or an empty string
   */
  void changeDatabase(const QString& readFileName, const QString& writeFileName);
  /**
   * @brief selectPlaybackAudioChannel selects an audio channel for playback
   * @param path the path of the audio file in the sample database
   * @param channel the channel number of the channel in the file
   */
  void selectPlaybackAudioChannel(const QString& path, const unsigned int channel);
  /**
   * @brief setPlaybackVolume sets the volume at which channels are played back
   * @param volume a number between 0 and 1
   */
  void setPlaybackVolume(const qreal volume);
  /**
   * @brief startPlayback starts or resumes audio playback if there is a channel selected
   */
  void startPlayback();
  /**
   * @brief stopPlayback pauses audio playback
   */
  void stopPlayback();
private:
  /// info about the audio playback device
  QAudioDeviceInfo audioDeviceInfo;
  /// the audio output
  QAudioOutput* audioOutput = nullptr;
  /// the buffer which acts as device for the audio output
  QBuffer audioOutputBuffer;
  /// the byte array that points to the playback audio channel
  QByteArray audioOutputArray;
  /// the open sample database
  SampleDatabase sampleDatabase;
};
