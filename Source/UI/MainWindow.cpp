/**
 * @file MainWindow.cpp implements methods for the main window class
 */

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>

#include "Detector/WhistleDetectorFactoryBase.hpp"
#include "Engine/WhistleLabEngine.hpp"

#include "LabelWidget.hpp"
#include "SampleDatabaseWidget.hpp"

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
  connect(fileMenu, &QMenu::aboutToShow, this, &MainWindow::updateFileMenu);
  updateFileMenu();

  evaluateMenu = menuBar()->addMenu(tr("&Evaluate"));
  auto detectorNames = WhistleDetectorFactoryBase::getDetectorNames();
  for (auto& name : detectorNames)
  {
    QAction* action = evaluateMenu->addAction(QString::fromStdString(name));
    connect(action, &QAction::triggered, this,
      [this, name]{ emit evaluateDetectorClicked(QString::fromStdString(name)); });
  }

  trainMenu = menuBar()->addMenu(tr("&Train"));
  for (auto& name : detectorNames)
  {
    QAction* action = trainMenu->addAction(QString::fromStdString(name));
    connect(action, &QAction::triggered, this,
      [this, name]{ emit trainDetectorClicked(QString::fromStdString(name)); });
  }

  viewMenu = menuBar()->addMenu(tr("&View"));
  connect(viewMenu, &QMenu::aboutToShow, this, &MainWindow::updateViewMenu);
  updateViewMenu();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  QAction* aboutAction = helpMenu->addAction(tr("&About"));
  connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
  QAction* aboutQtAction = helpMenu->addAction(tr("About &Qt"));
  connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

  sampleDatabaseWidget = new SampleDatabaseWidget(this);
  connect(this, &MainWindow::sampleDatabaseChanged,
    sampleDatabaseWidget, &SampleDatabaseWidget::updateSampleDatabase);
  connect(sampleDatabaseWidget, &SampleDatabaseWidget::channelSelectedForLabeling,
    this, &MainWindow::channelSelected);
  addDockWidget(Qt::LeftDockWidgetArea, sampleDatabaseWidget);

  labelWidget = new LabelWidget(this);
  connect(this, &MainWindow::channelChanged,
    labelWidget, &LabelWidget::updateChannel);
  connect(labelWidget, &LabelWidget::playClicked,
    this, &MainWindow::playClicked);
  connect(labelWidget, &LabelWidget::pauseClicked,
    this, &MainWindow::pauseClicked);
  addDockWidget(Qt::RightDockWidgetArea, labelWidget);

  setWindowTitle(tr("WhistleLab"));
  setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
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

  emit fileChanged(fileName, "");

  fileCloseAction->setEnabled(true);
}

void MainWindow::closeFile()
{
  fileCloseAction->setEnabled(false);

  emit fileChanged("", "");
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
      connect(action, &QAction::triggered, this, [this, file]{ openFile(file); });
    }
  }

  fileMenu->addSeparator();
  fileMenu->addAction(fileExitAction);
}

void MainWindow::updateViewMenu()
{
  viewMenu->clear();

  if (sampleDatabaseWidget != nullptr)
  {
    viewMenu->addAction(sampleDatabaseWidget->toggleViewAction());
  }

  if (labelWidget != nullptr)
  {
    viewMenu->addAction(labelWidget->toggleViewAction());
  }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  closeFile();
  QMainWindow::closeEvent(event);
}
