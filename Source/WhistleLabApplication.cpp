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
  connect(&mainWindow, &MainWindow::trainDetectorClicked, whistleLabEngine, &WhistleLabEngine::trainDetector);
  connect(&mainWindow, &MainWindow::channelSelected, whistleLabEngine, &WhistleLabEngine::selectChannel);
  connect(whistleLabEngine, &WhistleLabEngine::channelChanged, &mainWindow, &MainWindow::channelChanged);
  connect(&mainWindow, &MainWindow::playClicked, whistleLabEngine, &WhistleLabEngine::startPlayback);
  connect(&mainWindow, &MainWindow::pauseClicked, whistleLabEngine, &WhistleLabEngine::stopPlayback);
  connect(whistleLabEngine, &WhistleLabEngine::playbackPositionChanged, &mainWindow, &MainWindow::playbackPositionChanged);
  connect(whistleLabEngine, &WhistleLabEngine::evaluationDone, &mainWindow, &MainWindow::evaluationDone);

  workerThread->start(QThread::NormalPriority);

  mainWindow.show();
}

WhistleLabApplication::~WhistleLabApplication()
{
  workerThread->quit();
  workerThread->wait();
}
