/**
 * @file Main.cpp implements the main function
 */

#include "Engine/AudioChannel.hpp"
#include "Engine/SampleDatabase.hpp"

#include "WhistleLabApplication.hpp"


int main(int argc, char* argv[])
{
  qRegisterMetaType<AudioChannel>();
  qRegisterMetaType<SampleDatabase>();
  WhistleLabApplication app(argc, argv);

  return static_cast<QApplication&>(app).exec();
}
