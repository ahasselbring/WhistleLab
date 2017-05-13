/**
 * @file Main.cpp implements the main function
 */

#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>

#include "Detector/WhistleDetectorBase.hpp"
#include "Detector/WhistleDetectorFactoryBase.hpp"
#include "SampleDatabase.hpp"


int main(const int argc, const char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "An operation must be given!\n";
      return EXIT_FAILURE;
    }
    if (!std::strcmp(argv[1], "evaluate"))
    {
      if (argc < 3)
      {
        std::cerr << "A database must be given!\n";
        return EXIT_FAILURE;
      }
      SampleDatabase db(argv[2]);
      auto wd = WhistleDetectorFactoryBase::make("HULKsDetector");
      wd->evaluateOnDatabase(db);
    }
    else
    {
      std::cerr << "The operation " << argv[1] << "is not implemented!\n";
      return EXIT_FAILURE;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception caught in main: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
