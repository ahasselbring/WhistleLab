/**
 * @file WhistleLabApplication.hpp declares the whistle lab application class
 */

#pragma once

#include <QApplication>

#include "UI/MainWindow.hpp"


class QThread;
class WhistleLabEngine;

/**
 * @class WhistleLabApplication is the main application class
 */
class WhistleLabApplication : public QApplication
{
  Q_OBJECT
public:
  /**
   * @brief WhistleLabApplication initializes objects and starts threads
   * @param argc the number of command line arguments
   * @param argv the command line arguments
   */
  WhistleLabApplication(int& argc, char** argv);
  /**
   * @brief ~WhistleLabApplication destroys objects and stops threads
   */
  ~WhistleLabApplication();
private:
  /// the main UI object
  MainWindow mainWindow;
  /// the worker object
  WhistleLabEngine* whistleLabEngine = nullptr;
  /// the worker thread
  QThread* workerThread = nullptr;
};
