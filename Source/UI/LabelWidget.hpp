/**
 * @file LabelWidget.hpp declares the label widget class
 */

#pragma once

#include <QDockWidget>

#include "Engine/SampleDatabase.hpp"


class QString;
class QWidget;

/**
 * @class LabelWidget is a widget that allows labeling of audio data
 */
class LabelWidget : public QDockWidget
{
  Q_OBJECT
public:
  /**
   * @brief LabelWidget creates the label UI
   * @param parent the parent object
   */
  LabelWidget(QWidget* parent = 0);
signals:
  /**
   * @brief channelSelectedForPlayback is emitted when a channel is selected for playback
   * @param path the path of the audio file in the sample database
   * @param channel the channel number of the channel in the file
   */
  void channelSelectedForPlayback(const QString& path, const unsigned int channel);
public slots:
  /**
   * @brief updateSampleDatabase updates the sample database that is viewed
   * @param sampleDatabase a copy of the new sample database
   */
  void updateSampleDatabase(const SampleDatabase sampleDatabase);
  /**
   * @brief selectChannel selects a channel for editing in the label widget
   * @param path the path of the audio file in the sample database
   * @param channel the channel number of the channel in the file
   */
  void selectChannel(const QString& path, const unsigned int channel);
private:
  /// a local copy of the sample database
  SampleDatabase sampleDatabase;
};
