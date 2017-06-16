/**
 * @file NaoDevilsDetector.cpp implements methods of the NaoDevilsDetector class
 * The method is taken from and code based on:
 * https://github.com/NaoDevils/CodeRelease2016/blob/master/Src/Modules/Modeling/WhistleDetector/WhistleDetector.cpp
 */

#include <cassert>
#include <cmath>
#include <iostream>

#include "NaoDevilsDetector.hpp"


NaoDevilsDetector::NaoDevilsDetector()
  : realBuffer(windowSize)
  , complexBuffer(windowSize / 2 + 1)
  , fftPlan(fftw_plan_dft_r2c_1d(windowSize, realBuffer.data(), reinterpret_cast<fftw_complex*>(complexBuffer.data()), FFTW_ESTIMATE))
{
  static_assert(windowSize % 2 == 0, "The window size has to be even!");
}

NaoDevilsDetector::~NaoDevilsDetector()
{
  fftw_destroy_plan(fftPlan);
}

unsigned int NaoDevilsDetector::getPreferredBufferSize() const
{
  return windowSize;
}

bool NaoDevilsDetector::classify(const std::vector<float>& samples, const unsigned int sampleRate)
{
  if (samples.size() != windowSize)
  {
    std::cerr << "NaoDevilsDetector: Input sample size does not equal the desired size!\n";
    return false;
  }

  for (unsigned int i = 0; i < windowSize; i++)
  {
    realBuffer[i] = samples[i];
    if (useHannWindowing)
    {
      realBuffer[i] *= std::pow(std::sin(static_cast<float>(M_PI) * i / windowSize), 2.0f);
    }
  }

  fftw_execute(fftPlan);

  // This code is intentionally not that efficient/nice to keep it close to the original implementation.
  std::vector<double> amplitudes(ampSize);
  for (unsigned int i = 0; i < ampSize; i++)
  {
    const double abs2 = (complexBuffer[i].real() * complexBuffer[i].real() + complexBuffer[i].imag() * complexBuffer[i].imag());
    amplitudes[i] = std::sqrt(abs2);
  }

  unsigned int minI = static_cast<unsigned int>(minFrequency * windowSize / sampleRate);
  unsigned int maxI = static_cast<unsigned int>(maxFrequency * windowSize / sampleRate);
  assert(maxI < ampSize);
  unsigned int peakPos = minI;
  for (unsigned int i = minI; i <= maxI; i++)
  {
    if (amplitudes[i] > amplitudes[peakPos])
    {
      peakPos = i;
    }
  }
  if (amplitudes[peakPos] >= minAmp)
  {
    minI = static_cast<unsigned int>(peakPos * overtoneMultMin1);
    maxI = static_cast<unsigned int>(peakPos * overtoneMultMax1);
    assert(maxI < ampSize);
    unsigned int peak1Pos = minI;
    for (unsigned int i = minI; i <= maxI; i++)
    {
      if (amplitudes[i] > amplitudes[peak1Pos])
      {
        peak1Pos = i;
      }
    }
    if (amplitudes[peak1Pos] >= overtoneMinAmp1)
    {
      minI = static_cast<unsigned int>(peakPos * overtoneMultMin2);
      maxI = static_cast<unsigned int>(peakPos * overtoneMultMax2);
      assert(maxI < ampSize);
      unsigned int peak2Pos = minI;
      for (unsigned int i = minI; i <= maxI; i++)
      {
        if (amplitudes[i] > amplitudes[peak2Pos])
        {
          peak2Pos = i;
        }
      }
      if (amplitudes[peak2Pos] >= overtoneMinAmp2)
      {
        return true;
      }
    }
  }
  return false;
}
