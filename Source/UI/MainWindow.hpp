/**
 * @file MainWindow.hpp declares the main window class
 */

#pragma once

#include <QMainWindow>
#include <QSettings>
#include <QSignalMapper>
#include <QStringList>


class SampleDatabaseWidget;
class QAction;
class QCloseEvent;
class QMenu;
class QString;
class QWidget;
class QThread;
class WhistleLabEngine;

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
  /**
   * @brief ~MainWindow destroys the worker thread
   */
  ~MainWindow();
signals:
  /**
   * @brief fileChanged signals that a file has been opened or closed
   * @param readFileName the name of the new database file or an empty string
   * @param writeFileName the name where the old database is written or an empty string
   */
  void fileChanged(const QString& readFileName, const QString& writeFileName);
private slots:
  /**
   * @brief about shows a message box with infos about this program
   */
  void about();
  /**
   * @brief open is called by an open action
   */
  void open();
  /**
   * @brief openFile replaces the open sample database with another one loaded from a file
   * @param fileName the name of the file that should be loaded
   */
  void openFile(const QString& fileName);
  /**
   * @brief closeFile is called by a close action
   */
  void closeFile();
  /**
   * @brief updateFileMenu updates the recent files in the file menu
   */
  void updateFileMenu();
  /**
   * @brief updateViewMenu updates the view menu
   */
  void updateViewMenu();
private:
  /**
   * @brief closeEvent handles a window close event to close the file
   * @param event the close event
   */
  virtual void closeEvent(QCloseEvent* event) override;
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
  /// the menu containing view actions
  QMenu* viewMenu = nullptr;
  /// the menu containing help actions
  QMenu* helpMenu = nullptr;
  /// the settings of this application
  QSettings settings;
  /// the mapper that maps evaluate actions to a single evaluate slot
  QSignalMapper evaluateMapper;
  /// the mapper that maps open recent file actions to a single open file slot
  QSignalMapper recentFileMapper;
  /// the list of recent files
  QStringList recentFiles;
  /// the widget that views the sample database
  SampleDatabaseWidget* sampleDatabaseWidget = nullptr;
  /// the worker object
  WhistleLabEngine* whistleLabEngine = nullptr;
  /// the worker thread
  QThread* workerThread = nullptr;
};
