/**
 * @file MainWindow.hpp declares the main window class
 */

#pragma once

#include <QMainWindow>
#include <QSettings>
#include <QSignalMapper>
#include <QStringList>


class SampleDatabase;
class QAction;
class QCloseEvent;
class QMenu;
class QString;
class QWidget;

/**
 * @class MainWindow is the Qt main window class
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  /**
   * @brief MainWindow creates the main window UI
   * @param parent the parent object
   */
  MainWindow(QWidget* parent = 0);
private slots:
  /**
   * @brief open is called by an open action
   */
  void open();
  /**
   * @brief closeFile is called by a close action
   */
  void closeFile();
  /**
   * @brief evaluateDetector evaluates a detector on the currently opened database
   * @param name the name of the detector
   */
  void evaluateDetector(const QString& name);
private:
  /**
   * @brief closeEvent handles a window close event to close the file
   * @param event the close event
   */
  virtual void closeEvent(QCloseEvent* event) override;
  /**
   * @brief openFile replaces the open sample database with another one loaded from a file
   * @param fileName the name of the file that should be loaded
   */
  void openFile(const QString& fileName);
  /// the maximum number of entries in the recent files list
  static constexpr int maxNumberOfRecentFiles = 8;
  /// an action that creates a file open dialog
  QAction* fileOpenAction = nullptr;
  /// an action that closes a file
  QAction* fileCloseAction = nullptr;
  /// an action that terminates the program
  QAction* fileExitAction = nullptr;
  /// the menu containing file actions
  QMenu* fileMenu = nullptr;
  /// the menu containing evaluate actions
  QMenu* evaluateMenu = nullptr;
  /// the settings of this application
  QSettings settings;
  /// the mapper that maps evaluate actions to a single evaluate slot
  QSignalMapper evaluateMapper;
  /// the list of recent files
  QStringList recentFiles;
  /// the open sample database
  SampleDatabase* sampleDatabase = nullptr;
};
