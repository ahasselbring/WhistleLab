/**
 * @file WhistleLabEngine.cpp implements methods for the whistle lab engine class
 */

#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>
#include <QString>

#include "Detector/WhistleDetectorBase.hpp"
#include "Detector/WhistleDetectorFactoryBase.hpp"

#include "WhistleLabEngine.hpp"


WhistleLabEngine::WhistleLabEngine(QObject* parent)
  : QObject(parent)
  , audioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice())
{
  audioOutputBuffer.setBuffer(&audioOutputArray);
}

void WhistleLabEngine::evaluateDetector(const QString& name)
{
  if (!sampleDatabase.exists)
  {
    return;
  }

  auto detector = WhistleDetectorFactoryBase::make(name.toStdString());
  EvaluationResults results;
  detector->evaluateOnDatabase(sampleDatabase, &results);
  emit evaluationDone(results);
}

void WhistleLabEngine::trainDetector(const QString& name)
{
  if (!sampleDatabase.exists)
  {
    return;
  }

  auto detector = WhistleDetectorFactoryBase::make(name.toStdString());
  detector->trainOnDatabase(sampleDatabase);
}

void WhistleLabEngine::changeDatabase(const QString& readFileName, const QString& writeFileName)
{
  if (sampleDatabase.exists && !writeFileName.isEmpty())
  {
    sampleDatabase.writeToFile(writeFileName);
  }
  selectChannel("", 0);
  sampleDatabase.clear();
  if (!readFileName.isEmpty())
  {
    sampleDatabase.readFromFile(readFileName);
  }
  emit sampleDatabaseChanged(sampleDatabase);
}

void WhistleLabEngine::selectChannel(const QString& path, const unsigned int channel)
{
  if (!sampleDatabase.exists)
  {
    emit channelChanged(AudioChannel());
    Q_ASSERT(audioOutput == nullptr);
    return;
  }
  if (audioOutput != nullptr)
  {
    audioOutput->stop();
    audioOutputBuffer.close();
    audioOutputArray.setRawData(nullptr, 0);
    delete audioOutput;
    audioOutput = nullptr;
  }

  if (path.isEmpty())
  {
    emit channelChanged(AudioChannel());
    return;
  }

  // This method is assumed to be called with valid arguments, thus the asserts.
  for (auto& audioFile : sampleDatabase.audioFiles)
  {
    if (audioFile.path == path)
    {
      Q_ASSERT(channel < audioFile.numberOfChannels);
      for (auto& audioChannel : audioFile.channels)
      {
        if (audioChannel.channel == channel)
        {
          QAudioFormat format;
          format.setSampleRate(audioFile.sampleRate);
          format.setChannelCount(1);
          format.setSampleSize(8 * sizeof(float));
          format.setByteOrder(QAudioFormat::LittleEndian);
          format.setSampleType(QAudioFormat::Float);
          format.setCodec("audio/pcm");
          if (!audioDeviceInfo.isFormatSupported(format))
          {
            emit channelChanged(AudioChannel());
            return;
          }

          audioOutputArray.setRawData(reinterpret_cast<const char*>(audioChannel.samples.data()),
            static_cast<uint>(audioChannel.samples.size() * sizeof(float)));
          audioOutputBuffer.open(QIODevice::ReadOnly);

          audioOutput = new QAudioOutput(audioDeviceInfo, format, this);
          connect(audioOutput, &QAudioOutput::notify, this, &WhistleLabEngine::updatePlaybackPosition);
          audioOutput->setNotifyInterval(200);
          emit channelChanged(audioChannel);
          return;
        }
      }
      Q_ASSERT(false);
    }
  }
  Q_ASSERT(false);
}

void WhistleLabEngine::setPlaybackVolume(const qreal volume)
{
  if (audioOutput != nullptr)
  {
    audioOutput->setVolume(volume);
  }
}

void WhistleLabEngine::startPlayback()
{
  if (audioOutput != nullptr)
  {
    if (audioOutput->state() == QAudio::SuspendedState)
    {
      audioOutput->resume();
    }
    else
    {
      audioOutput->start(&audioOutputBuffer);
    }
  }
}

void WhistleLabEngine::stopPlayback()
{
  if (audioOutput != nullptr)
  {
    audioOutput->suspend();
  }
}

void WhistleLabEngine::updatePlaybackPosition()
{
  Q_ASSERT(audioOutput != nullptr);
  emit playbackPositionChanged(static_cast<unsigned int>(audioOutput->processedUSecs() * audioOutput->format().sampleRate() / 1000000));
}
