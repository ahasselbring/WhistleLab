/**
 * @file EvaluationHandle.cpp implements methods of the EvaluationHandle class
 */

#include <cstring>

#ifdef __linux__
#include <time.h>
#endif

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
  if (timeWhenLastRead != 0)
  {
    const std::uint64_t timeWhenFinished = getCurrentThreadTime();
    const float executionTimePerDuration =
      (static_cast<float>(timeWhenFinished - timeWhenLastRead) / 1000000000.f)
        / (static_cast<float>(length) / static_cast<float>(af.sampleRate));
    executionTimes.push_back(executionTimePerDuration);
  }
  if (pos + length > static_cast<unsigned int>(af.channels[0].samples.size()))
  {
    length = af.channels[0].samples.size() - pos;
  }
  std::memcpy(buf, af.channels[0].samples.data() + pos, length * sizeof(float));
  pos += length;
  timeWhenLastRead = getCurrentThreadTime();
  return length;
}

void EvaluationHandle::report(int offset)
{
  detectionPositions.push_back(pos);
  detections.push_back(pos + offset);
}

int EvaluationHandle::insideWhistle(int offset) const
{
  int actualPosition = pos + offset;
  for (auto& wl : af.channels[0].whistleLabels)
  {
    if (wl.start < actualPosition && actualPosition < wl.end)
    {
      return actualPosition - wl.start;
    }
  }
  return 0;
}

std::uint64_t EvaluationHandle::getCurrentThreadTime()
{
#ifdef __linux__
  timespec ts;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
  return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#else
  // Runtime measurements are currently not possible on non-Linux systems.
  return 0;
#endif
}
