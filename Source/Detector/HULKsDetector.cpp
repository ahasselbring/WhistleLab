#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>

#include "HULKsDetector.hpp"


HULKsDetector::HULKsDetector()
  : realBuffer(bufferSize)
  , complexBuffer(bufferSize)
  , fftPlan(fftw_plan_dft_r2c_1d(bufferSize, realBuffer.data(), reinterpret_cast<fftw_complex*>(complexBuffer.data()), FFTW_ESTIMATE))
{
}

HULKsDetector::~HULKsDetector()
{
  fftw_destroy_plan(fftPlan);
}

unsigned int HULKsDetector::getPreferredBufferSize() const
{
  return bufferSize;
}

bool HULKsDetector::classify(const std::vector<float>& samples, const unsigned int sampleRate)
{
  if (samples.size() != bufferSize)
  {
    std::cerr << "HULKsDetector: Input sample size does not equal the desired size!\n";
    return false;
  }
  std::copy(samples.begin(), samples.end(), realBuffer.begin());
  fftw_execute(fftPlan);

  const std::vector<std::complex<double>>& freqData = complexBuffer;
  const double freqResolution = static_cast<double>(sampleRate) / bufferSize;
  const unsigned int minFreqIndex = std::ceil(minFrequency / freqResolution);
  const unsigned int maxFreqIndex = std::ceil(maxFrequency / freqResolution);

  if (maxFreqIndex > bufferSize)
  {
    std::cerr << "HULKsDetector: maxFreqIndex " << maxFreqIndex << " is larger than the Nyquist frequency!\n";
    return false;
  }

  double power = 0;
  double stopBandPower = 0;

  for (unsigned int i = minFreqIndex; i < freqData.size(); i++)
  {
    // The division by freqResolution is not strictly necessary since it cancels out in the division below.
    double abs2 = (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) / freqResolution;
    if (i < maxFreqIndex)
    {
      power += abs2;
    }
    else
    {
      stopBandPower += abs2;
    }
  }

  return power / stopBandPower > threshold;
}
