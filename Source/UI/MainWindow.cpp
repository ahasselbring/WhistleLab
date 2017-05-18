/**
 * @file MainWindow.cpp implements methods for the MainWindow class
 */

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QString>

#include "SampleDatabase.hpp"

#include "MainWindow.hpp"


MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , settings("HULKs", "WhistleLab")
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

void MainWindow::closeEvent(QCloseEvent* event)
{
  closeFile();
  QMainWindow::closeEvent(event);
}

void MainWindow::openFile(const QString& fileName)
{
  closeFile();

  fileCloseAction->setEnabled(true);

  sampleDatabase = new SampleDatabase(fileName.toStdString());
}
