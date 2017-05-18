/**
 * @file MainWindow.hpp declares the main window class
 */

#pragma once

#include <QMainWindow>
#include <QSettings>


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
  /// an action that creates a file open dialog
  QAction* fileOpenAction = nullptr;
  /// an action that closes a file
  QAction* fileCloseAction = nullptr;
  /// an action that terminates the program
  QAction* fileExitAction = nullptr;
  /// the menu containing file actions
  QMenu* fileMenu = nullptr;
  /// the settings of this application
  QSettings settings;
  /// the open sample database
  SampleDatabase* sampleDatabase = nullptr;
};
