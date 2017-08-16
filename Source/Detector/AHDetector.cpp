/**
 * @file AHDetector.cpp implements methods of the AHDetector class
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>

#include "AHDetector.hpp"


AHDetector::AHDetector()
  : realBuffer(bufferSize)
  , complexBuffer(bufferSize / 2 + 1)
  , fftPlan(fftw_plan_dft_r2c_1d(bufferSize, realBuffer.data(), reinterpret_cast<fftw_complex*>(complexBuffer.data()), FFTW_ESTIMATE))
{
  static_assert(bufferSize % 2 == 0, "The buffer size has to be even!");
}

AHDetector::~AHDetector()
{
  fftw_destroy_plan(fftPlan);
}

void AHDetector::evaluate(EvaluationHandle& eh)
{
  std::array<float, bufferSize> samples;
  const std::vector<std::complex<double>>& freqData = complexBuffer;
  const double freqResolution = static_cast<double>(bufferSize) / eh.getSampleRate();
  const unsigned int minFreqIndex = static_cast<unsigned int>(std::ceil(minFrequency * freqResolution));
  const unsigned int maxFreqIndex = static_cast<unsigned int>(std::ceil(maxFrequency * freqResolution));
  if (maxFreqIndex >= complexBuffer.size())
  {
    std::cerr << "AHDetector: maxFreqIndex " << maxFreqIndex << " is larger than the Nyquist frequency!\n";
    return;
  }

  while (eh.readSingleChannel(samples.data(), bufferSize) == bufferSize)
  {
    std::copy(samples.begin(), samples.end(), realBuffer.begin());
    fftw_execute(fftPlan);

    double whistlePower[2] = { 0, 0 };
    double stopBandPower[2] = { 0, 0 };
    double maxPower = minRequiredPower;
    int maxPowerFreqIndex = -1;

    for (unsigned int i = minFreqIndex; i < maxFreqIndex; i++)
    {
      // The multiplication by freqResolution is not strictly necessary since it cancels out in the division below.
      const double abs2 = (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) * freqResolution;
      if (abs2 > maxPower)
      {
        maxPower = abs2;
        maxPowerFreqIndex = i;
      }
      whistlePower[0] += abs2;
    }
    if (maxPowerFreqIndex == -1)
    {
      continue;
    }
    int i2 = (maxFreqIndex - minFreqIndex) / 2;
    const int minFreqIndex2 = maxPowerFreqIndex * 2 - i2 / 2;
    const int maxFreqIndex2 = maxPowerFreqIndex * 2 + i2 / 2;
    assert(minFreqIndex2 >= 0 && maxFreqIndex2);
    for (int i = maxFreqIndex; i < minFreqIndex2; i++)
    {
      stopBandPower[0] += (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) * freqResolution;
    }
    for (int i = minFreqIndex2; i < maxFreqIndex2; i++)
    {
      whistlePower[1] += (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) * freqResolution;
    }
    for (int i = maxFreqIndex2; i < static_cast<int>(freqData.size()); i++)
    {
      stopBandPower[1] += (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) * freqResolution;
    }
    const double whistleBandRange = maxFreqIndex - minFreqIndex;
    const double stopBandRange = minFreqIndex2 - maxFreqIndex;
    const double whistleBandRange2 = maxFreqIndex2 - minFreqIndex2;
    const double stopBandRange2 = static_cast<double>(freqData.size()) - maxFreqIndex2;
    if ((whistlePower[0] / whistleBandRange) < (stopBandPower[0] / stopBandRange) * whistleBand0OverStopBand0)
    {
      continue;
    }
    if ((whistlePower[1] / whistleBandRange2) < (stopBandPower[0] / stopBandRange) * whistleBand1OverStopBand0)
    {
      continue;
    }
    if ((whistlePower[0] + whistlePower[1]) / (whistleBandRange + whistleBandRange2)
      < (stopBandPower[0] + stopBandPower[1]) / (stopBandRange + stopBandRange2) * whistleBandOverStopBand)
    {
      continue;
    }
    eh.report(-static_cast<int>(bufferSize) / 2);
  }
}
