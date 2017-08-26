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
  , fftPlan(fftwf_plan_dft_r2c_1d(windowSize, realBuffer.data(), reinterpret_cast<fftwf_complex*>(complexBuffer.data()), FFTW_ESTIMATE))
{
  static_assert(windowSize % 2 == 0, "The window size has to be even!");
}

NaoDevilsDetector::~NaoDevilsDetector()
{
  fftwf_destroy_plan(fftPlan);
}

void NaoDevilsDetector::evaluate(EvaluationHandle& eh)
{
  unsigned int attackCount = 0, releaseCount = release, ringPos = 0;
  std::vector<float> buffer(windowSize, 0.0f);
  // This leads to a swapped buffer in every second cycle (i.e. the first half of the buffer was recorded after the second half).
  // This is the same way as in the original Nao Devils implementations and has no effect as only the absolute value of the
  // Fourier transform is used.
  while (eh.readSingleChannel(buffer.data() + ringPos, windowSize / 2) == windowSize / 2)
  {
    bool detected = false;
    ringPos += windowSize / 2;
    ringPos %= windowSize;
    for (unsigned int i = 0; i < windowSize; i++)
    {
      realBuffer[i] = buffer[i];
      if (useHannWindowing)
      {
        realBuffer[i] *= std::pow(std::sin(static_cast<float>(M_PI) * static_cast<float>(i) / windowSize), 2.0f);
      }
    }

    fftwf_execute(fftPlan);

    // This code is intentionally not that efficient/nice to keep it close to the original implementation.
    std::vector<float> amplitudes(ampSize);
    for (unsigned int i = 0; i < ampSize; i++)
    {
      const float
        abs2 = (complexBuffer[i].real() * complexBuffer[i].real() + complexBuffer[i].imag() * complexBuffer[i].imag());
      amplitudes[i] = std::sqrt(abs2);
    }

    unsigned int minI = minFrequency * windowSize / eh.getSampleRate();
    unsigned int maxI = maxFrequency * windowSize / eh.getSampleRate();
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
      minI = static_cast<unsigned int>(static_cast<float>(peakPos) * overtoneMultMin1);
      maxI = static_cast<unsigned int>(static_cast<float>(peakPos) * overtoneMultMax1);
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
        minI = static_cast<unsigned int>(static_cast<float>(peakPos) * overtoneMultMin2);
        maxI = static_cast<unsigned int>(static_cast<float>(peakPos) * overtoneMultMax2);
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
          attackCount++;
          if (attackCount >= attack)
          {
            detected = true;
          }
        }
      }
    }
    if (detected)
    {
      releaseCount = 0;
      eh.report(-static_cast<int>(windowSize) / 2);
    }
    else if (releaseCount < release)
    {
      releaseCount++;
      // The following line is not present in the original Nao Devils code.
      // They would reset attackCount only when not in the SET state (I think that is a bug in their code).
      attackCount = 0;
      eh.report(-static_cast<int>(windowSize) / 2);
    }
  }
}
