/**
 * @file LabelWidget.hpp declares the label widget class
 */

#pragma once

#include <QDockWidget>

#include "Engine/AudioChannel.hpp"


class QHBoxLayout;
class QPushButton;
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
   * @brief playClicked is emitted when the play button is clicked
   */
  void playClicked();
  /**
   * @brief pauseClicked is emitted when the pause button is clicked
   */
  void pauseClicked();
public slots:
  /**
   * @brief updateChannel updates the channel that is labeled
   * @param audioChannel a reference to the new audio channel
   */
  void updateChannel(const AudioChannel& audioChannel);
  /**
   * @brief updatePlaybackPosition updates the playback position
   * @param pos the new playback position in samples
   */
  void updatePlaybackPosition(const unsigned int pos);
private:
  /// the layout that contains the buttons
  QHBoxLayout* buttonLayout = nullptr;
  /// a widget that contains the layout because a dock widget already has a layout
  QWidget* layoutWidget = nullptr;
  /// the play button
  QPushButton* playButton = nullptr;
  /// the pause button
  QPushButton* pauseButton = nullptr;
};
