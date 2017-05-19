/**
 * @file MainWindow.cpp implements methods for the MainWindow class
 */

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>

#include "Detector/WhistleDetectorBase.hpp"
#include "Detector/WhistleDetectorFactoryBase.hpp"
#include "SampleDatabase.hpp"

#include "MainWindow.hpp"
#include "SampleDatabaseWidget.hpp"


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
  connect(fileMenu, &QMenu::aboutToShow, this, &MainWindow::updateFileMenu);
  connect(&recentFileMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), this, &MainWindow::openFile);
  updateFileMenu();

  evaluateMenu = menuBar()->addMenu(tr("E&valuate"));
  connect(&evaluateMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), this, &MainWindow::evaluateDetector);
  auto detectorNames = WhistleDetectorFactoryBase::getDetectorNames();
  for (auto& name : detectorNames)
  {
    QAction* action = evaluateMenu->addAction(QString::fromStdString(name));
    evaluateMapper.setMapping(action, QString::fromStdString(name));
    connect(action, &QAction::triggered, &evaluateMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
  }

  helpMenu = menuBar()->addMenu(tr("&Help"));
  QAction* aboutAction = helpMenu->addAction(tr("&About"));
  connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
  helpMenu->addAction(aboutAction);
  QAction* aboutQtAction = helpMenu->addAction(tr("About &Qt"));
  connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
  helpMenu->addAction(aboutQtAction);

  setWindowTitle(tr("WhistleLab"));
  setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About"), tr("WhistleLab 4.1 Ultimate Edition Platinum"));
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

  Q_ASSERT(sampleDatabase == nullptr);
  sampleDatabase = new SampleDatabase(fileName.toStdString());

  Q_ASSERT(sampleDatabaseWidget == nullptr);
  sampleDatabaseWidget = new SampleDatabaseWidget(this);
  addDockWidget(Qt::LeftDockWidgetArea, sampleDatabaseWidget);
}

void MainWindow::closeFile()
{
  delete sampleDatabaseWidget;
  sampleDatabaseWidget = nullptr;

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

void MainWindow::updateFileMenu()
{
  fileMenu->clear();
  fileMenu->addAction(fileOpenAction);
  fileMenu->addAction(fileCloseAction);

  if (!recentFiles.isEmpty())
  {
    fileMenu->addSeparator();
    char shortcut = '1';
    for (auto& file : recentFiles)
    {
      QAction* action = fileMenu->addAction("&" + QString(shortcut++) + " " + QFileInfo(file).fileName());
      recentFileMapper.setMapping(action, file);
      connect(action, &QAction::triggered, &recentFileMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
    }
  }

  fileMenu->addSeparator();
  fileMenu->addAction(fileExitAction);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  closeFile();
  QMainWindow::closeEvent(event);
}
