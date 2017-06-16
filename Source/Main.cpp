/**
 * @file Main.cpp implements the main function
 */

#include "Engine/SampleDatabase.hpp"

#include "WhistleLabApplication.hpp"


int main(int argc, char* argv[])
{
  qRegisterMetaType<SampleDatabase>();
  WhistleLabApplication app(argc, argv);

  return static_cast<QApplication&>(app).exec();
}
