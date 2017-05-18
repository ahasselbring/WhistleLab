/**
 * @file MainWindow.cpp implements methods for the MainWindow class
 */

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QString>

#include "Detector/WhistleDetectorBase.hpp"
#include "Detector/WhistleDetectorFactoryBase.hpp"
#include "SampleDatabase.hpp"

#include "MainWindow.hpp"


MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , settings("HULKs", "WhistleLab")
  , recentFiles(settings.value("RecentFiles").toStringList())
{
  fileOpenAction = new QAction(tr("&Open"), this);
  connect(fileOpenAction, &QAction::triggered, this, &MainWindow::open);

  fileCloseAction = new QAction(tr("&Close"), this);
  fileCloseAction->setEnabled(false);
  connect(fileCloseAction, &QAction::triggered, this, &MainWindow::closeFile);

  fileExitAction = new QAction(tr("E&xit"), this);
  connect(fileExitAction, &QAction::triggered, this, &QWidget::close);

  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(fileOpenAction);
  fileMenu->addAction(fileCloseAction);
  fileMenu->addAction(fileExitAction);

  evaluateMenu = menuBar()->addMenu(tr("E&valuate"));
  connect(&evaluateMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), this, &MainWindow::evaluateDetector);
  auto detectorNames = WhistleDetectorFactoryBase::getDetectorNames();
  for (auto& name : detectorNames)
  {
    QAction* action = evaluateMenu->addAction(QString::fromStdString(name));
    evaluateMapper.setMapping(action, QString::fromStdString(name));
    connect(action, &QAction::triggered, &evaluateMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
  }

  setWindowTitle(tr("WhistleLab"));
  setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), settings.value("OpenDirectory", "").toString());
  if (fileName.isEmpty())
  {
    return;
  }
  settings.setValue("OpenDirectory", QFileInfo(fileName).dir().path());

  openFile(fileName);
}

void MainWindow::closeFile()
{
  delete sampleDatabase;
  sampleDatabase = nullptr;

  fileCloseAction->setEnabled(false);
}

void MainWindow::evaluateDetector(const QString& name)
{
  if (sampleDatabase == nullptr)
  {
    return;
  }

  auto detector = WhistleDetectorFactoryBase::make(name.toStdString());
  detector->evaluateOnDatabase(*sampleDatabase);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  closeFile();
  QMainWindow::closeEvent(event);
}

void MainWindow::openFile(const QString& fileName)
{
  closeFile();

  QFileInfo fileInfo(fileName);
  QString filePath = fileInfo.absoluteDir().canonicalPath() + '/' + fileInfo.fileName();

  recentFiles.removeAll(filePath);

  if (!fileInfo.exists())
  {
    settings.setValue("RecentFiles", recentFiles);
    return;
  }

  recentFiles.prepend(filePath);
  while (recentFiles.size() > maxNumberOfRecentFiles)
  {
    recentFiles.removeLast();
  }
  settings.setValue("RecentFiles", recentFiles);

  fileCloseAction->setEnabled(true);

  sampleDatabase = new SampleDatabase(fileName.toStdString());
}
