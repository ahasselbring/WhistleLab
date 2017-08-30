/**
 * @file AHDetector.cpp implements methods of the AHDetector class
 */

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <iostream>
#include <limits>

#include "AHDetector.hpp"


AHDetector::AHDetector()
  : realBuffer(bufferSize)
  , complexBuffer(bufferSize / 2 + 1)
  , powerBuffer(bufferSize / 2 + 1)
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
    // 1. Perform discrete fourier (with Hann window) transform to obtain frequency spectrum.
    for (unsigned int i = 0; i < realBuffer.size(); i++)
    {
      realBuffer[i] = samples[i];
      realBuffer[i] *= std::pow(std::sin(static_cast<float>(M_PI) * static_cast<float>(i) / bufferSize), 2.0f);
    }
    fftw_execute(fftPlan);

    // 2. Precompute the absolute values (hopefully) normalized by buffer size and sample rate.
    for (unsigned int i = 0; i < freqData.size(); i++)
    {
      powerBuffer[i] = (freqData[i].real() * freqData[i].real() + freqData[i].imag() * freqData[i].imag()) / freqResolution;
    }

    // 3. Find the frequency at which the power is highest in a configurable band.
    double maxPower = minRequiredPower;
    unsigned int maxPowerFreqIndex = 0;
    for (unsigned int i = minFreqIndex; i < maxFreqIndex; i++)
    {
      const double abs2 = powerBuffer[i];
      if (abs2 > maxPower)
      {
        maxPower = abs2;
        maxPowerFreqIndex = i;
      }
    }

    // 4. If a configurable power has not been surpassed, the buffer is rejected.
    if (maxPowerFreqIndex == 0)
    {
      continue;
    }

    // 5. Determine power in the range of the base frequency while detecting its boundaries.
    double whistlePower[2] = { powerBuffer[maxPowerFreqIndex], 0 };
    double stopBandPower[2] = { 0, 0 };
    const unsigned int i2 = (maxFreqIndex - minFreqIndex) / 2;
    const unsigned int lowerBoundLowerBound = std::max(maxPowerFreqIndex - i2, 1U);
    const unsigned int upperBoundUpperBound = std::min(maxPowerFreqIndex + i2, static_cast<unsigned int>(freqData.size()));
    unsigned int upperBound, lowerBound;
    unsigned int lowerBoundAtMinLowerPower = 0, upperBoundAtMinUpperPower = 0;
    double minLowerPower = std::numeric_limits<double>::max(), minUpperPower = std::numeric_limits<double>::max();
    double whistlePowerAtMinLowerPower = 0.f, whistlePowerAtMinUpperPower = 0.f;
    for (lowerBound = maxPowerFreqIndex - 1; lowerBound > lowerBoundLowerBound; lowerBound--)
    {
      whistlePower[0] += powerBuffer[lowerBound];
      if (powerBuffer[lowerBound] < maxPower * minPowerOverMaxPower)
      {
        break;
      }
      if (powerBuffer[lowerBound] < minLowerPower)
      {
        minLowerPower = powerBuffer[lowerBound];
        lowerBoundAtMinLowerPower = lowerBound;
        whistlePowerAtMinLowerPower = whistlePower[0];
      }
    }
    if (lowerBound == lowerBoundLowerBound)
    {
      lowerBound = lowerBoundAtMinLowerPower;
      whistlePower[0] = whistlePowerAtMinLowerPower;
    }
    for (upperBound = maxPowerFreqIndex + 1; upperBound < upperBoundUpperBound; upperBound++)
    {
      whistlePower[0] += powerBuffer[upperBound];
      if (powerBuffer[upperBound] < maxPower * minPowerOverMaxPower)
      {
        break;
      }
      if (powerBuffer[upperBound] < minUpperPower)
      {
        minUpperPower = powerBuffer[upperBound];
        upperBoundAtMinUpperPower = upperBound;
        whistlePowerAtMinUpperPower = whistlePower[0];
      }
    }
    if (upperBound == upperBoundUpperBound)
    {
      upperBound = upperBoundAtMinUpperPower;
      whistlePower[0] = whistlePowerAtMinUpperPower;
    }
    assert(upperBound > lowerBound);

    // 6. Determine power in the rest the second harmonic band and in between and above.
    const int minFreqIndex2 = 2 * maxPowerFreqIndex - (upperBound - lowerBound) / 4;
    const int maxFreqIndex2 = 2 * maxPowerFreqIndex + (upperBound - lowerBound) / 4;
    assert(minFreqIndex2 >= 0 && maxFreqIndex2 <= freqData.size());
    for (int i = upperBound; i < minFreqIndex2; i++)
    {
      stopBandPower[0] += powerBuffer[i];
    }
    for (int i = minFreqIndex2; i < maxFreqIndex2; i++)
    {
      whistlePower[1] += powerBuffer[i];
    }
    for (int i = maxFreqIndex2; i < static_cast<int>(freqData.size()); i++)
    {
      stopBandPower[1] += powerBuffer[i];
    }

    // 7. Normalize the power to their ranges.
    const double whistleBandRange = std::max(upperBound - lowerBound, 1U);
    const double stopBandRange = std::max(minFreqIndex2 - upperBound, 1U);
    const double whistleBandRange2 = std::max(maxFreqIndex2 - minFreqIndex2, 1);
    const double stopBandRange2 = std::max(static_cast<int>(freqData.size()) - maxFreqIndex2, 1);
    whistlePower[0] /= whistleBandRange;
    whistlePower[1] /= whistleBandRange2;
    stopBandPower[0] /= stopBandRange;
    stopBandPower[1] /= stopBandRange2;

    // 8. Compute feature vector.
    std::array<double, numOfFeatures> features;
    features[0] = maxPower;
    features[1] = whistlePower[0] / stopBandPower[0];
    features[2] = whistlePower[1] / stopBandPower[0];
    features[3] = (whistlePower[0] + whistlePower[1]) / (stopBandPower[0] + stopBandPower[1]);

    // 9. Run classifier.
    if (classify(features))
    {
      eh.report(-static_cast<int>(bufferSize) / 2);
    }
  }
}

void AHDetector::trainOnDatabase(const SampleDatabase&)
{
}

bool AHDetector::classify(const std::array<double, numOfFeatures>& features) const
{
  if (features[0] <= 1079858)
  {
    if (features[3] <= 332.149 && features[2] > 2.99929)
    {
      return true;
    }
  }
  else
  {
    return true;
  }
  return false;
}
