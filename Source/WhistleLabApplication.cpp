/**
 * @file WhistleLabApplication.cpp implements methods for the whistle lab application class
 */

#include <QThread>

#include "Engine/WhistleLabEngine.hpp"
#include "UI/MainWindow.hpp"

#include "WhistleLabApplication.hpp"


WhistleLabApplication::WhistleLabApplication(int& argc, char** argv)
  : QApplication(argc, argv)
{
  setApplicationName("WhistleLab");

  workerThread = new QThread(this);
  whistleLabEngine = new WhistleLabEngine;
  whistleLabEngine->moveToThread(workerThread);

  connect(&mainWindow, &MainWindow::fileChanged, whistleLabEngine, &WhistleLabEngine::changeDatabase);
  connect(whistleLabEngine, &WhistleLabEngine::sampleDatabaseChanged, &mainWindow, &MainWindow::sampleDatabaseChanged);
  connect(&mainWindow, &MainWindow::evaluateDetectorClicked, whistleLabEngine, &WhistleLabEngine::evaluateDetector);
  connect(&mainWindow, &MainWindow::channelSelectedForPlayback, whistleLabEngine, &WhistleLabEngine::selectPlaybackAudioChannel);

  workerThread->start(QThread::NormalPriority);

  mainWindow.show();
}

WhistleLabApplication::~WhistleLabApplication()
{
  workerThread->quit();
  workerThread->wait();
}
