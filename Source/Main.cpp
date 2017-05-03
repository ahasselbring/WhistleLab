/**
 * @file Main.cpp implements the main function
 */

#include <cstdlib>
#include <exception>
#include <iostream>

#include "Detector/HULKsDetector.hpp"
#include "Detector/WhistleDetectorInterface.hpp"
#include "SampleDatabase.hpp"


int main(const int argc, const char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "A path to a sample database needs to be passed!";
    return EXIT_FAILURE;
  }
  WhistleDetectorInterface* wd = nullptr;
  try
  {
    SampleDatabase db(argv[1]);
    wd = new HULKsDetector;
    wd->evaluateOnDatabase(db);
  }
  catch (const std::exception& e)
  {
    delete wd;
    std::cerr << "Exception caught in main: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  delete wd;
  return EXIT_SUCCESS;
}
