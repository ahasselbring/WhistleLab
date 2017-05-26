/**
 * @file Main.cpp implements the main function
 */

#include <QApplication>

#include "Engine/SampleDatabase.hpp"
#include "UI/MainWindow.hpp"


int main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  qRegisterMetaType<SampleDatabase>();
  app.setApplicationName("WhistleLab");

  MainWindow window;
  window.show();

  return app.exec();
}
