/**
 * @file Main.cpp implements the main function
 */

#include <cstdlib>
#include <exception>
#include <iostream>

#include "SampleDatabase.hpp"


int main(const int argc, const char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "A path to a sample database needs to be passed!";
    return EXIT_FAILURE;
  }
  try
  {
    SampleDatabase db(argv[1]);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception caught in main: " << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
