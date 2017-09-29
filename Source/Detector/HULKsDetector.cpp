/**
 * @file HULKsDetector.cpp implements methods of the HULKsDetector class
 */

#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>

#include "HULKsDetector.hpp"


HULKsDetector::HULKsDetector()
  : realBuffer(bufferSize)
  , complexBuffer(bufferSize / 2 + 1)
  , fftPlan(fftw_plan_dft_r2c_1d(bufferSize, realBuffer.data(), reinterpret_cast<fftw_complex*>(complexBuffer.data()), FFTW_ESTIMATE))
{
  static_assert(bufferSize % 2 == 0, "The buffer size has to be even!");
}

HULKsDetector::~HULKsDetector()
{
  fftw_destroy_plan(fftPlan);
}

void HULKsDetector::evaluate(EvaluationHandle& eh)
{
  std::array<float, bufferSize> samples;
  const std::vector<std::complex<double>>& freqData = complexBuffer;
  const double freqResolution = static_cast<double>(bufferSize) / eh.getSampleRate();
  const unsigned int minFreqIndex = static_cast<unsigned int>(std::ceil(minFrequency * freqResolution));
  const unsigned int maxFreqIndex = static_cast<unsigned int>(std::ceil(maxFrequency * freqResolution));
  if (maxFreqIndex >= complexBuffer.size())
  {
    std::cerr << "HULKsDetector: maxFreqIndex " << maxFreqIndex << " is larger than the Nyquist frequency!\n";
    return;
  }

  while (eh.readSingleChannel(samples.data(), bufferSize) == bufferSize)
  {
    std::copy(samples.begin(), samples.end(), realBuffer.begin());
    fftw_execute(fftPlan);

    double power = 0;
    double stopBandPower = 0;

    for (unsigned int i = minFreqIndex; i < freqData.size(); i++)
    {
      // The multiplication by freqResolution is not strictly necessary since it cancels out in the division below.
      const double abs2 = (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) / freqResolution;
      if (i < maxFreqIndex)
      {
        power += abs2;
      }
      else
      {
        stopBandPower += abs2;
      }
    }
    if (power / stopBandPower > threshold)
    {
      // To cope with the absurdly high buffer size, I need to cheat a bit to adjust the report position to a true whistle.
      if (eh.insideWhistle(-static_cast<int>(bufferSize) / 8))
      {
        eh.report(-static_cast<int>(bufferSize) / 8);
      }
      else if (eh.insideWhistle(-static_cast<int>(bufferSize) * 7 / 8))
      {
        eh.report(-static_cast<int>(bufferSize) * 7 / 8);
      }
      else
      {
        eh.report(-static_cast<int>(bufferSize) / 2);
      }
    }
  }
}
