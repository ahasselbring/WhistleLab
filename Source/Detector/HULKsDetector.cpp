#include "HULKsDetector.hpp"


unsigned int HULKsDetector::getPreferredBufferSize()
{
  return 8192;
}

bool HULKsDetector::classify(const std::vector<float>&, const unsigned int)
{
  return true;
}
