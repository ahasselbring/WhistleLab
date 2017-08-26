/**
 * @file EvaluationHandle.cpp implements methods of the EvaluationHandle class
 */

#include <cstring>

#include "EvaluationHandle.hpp"


EvaluationHandle::EvaluationHandle(const AudioFile& af)
  : af(af)
{
}

unsigned int EvaluationHandle::getSampleRate() const
{
  return af.sampleRate;
}

unsigned int EvaluationHandle::getNumberOfChannels() const
{
  return af.numberOfChannels;
}

unsigned int EvaluationHandle::readSingleChannel(float* buf, unsigned int length)
{
  if (pos + length > static_cast<unsigned int>(af.channels[0].samples.size()))
  {
    length = af.channels[0].samples.size() - pos;
  }
  std::memcpy(buf, af.channels[0].samples.data() + pos, length * sizeof(float));
  pos += length;
  return length;
}

void EvaluationHandle::report(int offset)
{
  detections.push_back(pos + offset);
}
