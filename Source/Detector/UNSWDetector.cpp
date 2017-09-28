/**
 * @file UNSWDetector.cpp implements methods of the UNSWDetector class
 * The method is taken from and code based on:
 * https://github.com/UNSWComputing/rUNSWift-2016-release/blob/master/image/home/nao/whistle/whistle_detector.py
 * which in turn is based on the detector of the SPL team Austrian Kangaroos:
 * http://www.austrian-kangaroos.com/public/code/WhistleDetector.tgz
 */

#include <algorithm>
#include <cassert>
#include <cmath>

#include "UNSWDetector.hpp"


UNSWDetector::UNSWDetector()
{
}

void UNSWDetector::evaluate(EvaluationHandle& eh)
{
  state.setSampleRate(eh.getSampleRate());
  state.reset();
  state.currentCounter = 0;
  state.counterWhenWhistleStarted = 0;
  state.statsMemory.clear();
  window.resize(windowSize);
  spectrum.resize(windowSize / 2 + 1);
  fftPlan = fftwf_plan_dft_r2c_1d(windowSize, window.data(), reinterpret_cast<fftwf_complex*>(spectrum.data()), FFTW_ESTIMATE);
  while (eh.readSingleChannel(window.data(), windowSize) == windowSize)
  {
    fftwf_execute(fftPlan);
    state.interrogate(spectrum);
    if (state.whistleDone)
    {
      eh.report(-static_cast<int>((state.currentCounter - state.counterWhenWhistleStarted - 2) * windowSize));
    }
  }
  fftwf_destroy_plan(fftPlan);
}

UNSWDetector::WhistleState::WhistleState()
{
  reset();
}

void UNSWDetector::WhistleState::setSampleRate(const unsigned int sampleRate)
{
  spectrumWhistleBegin = fWhistleBegin * windowSize / sampleRate;
  spectrumWhistleEnd = fWhistleEnd * windowSize / sampleRate;
  statsRemember = (sampleRate + windowSize / 2) / windowSize;
  nWhistleOkaySpectra = static_cast<unsigned int>(whistleOkayTime * static_cast<float>(sampleRate) / static_cast<float>(windowSize) + 0.5f);
  nWhistleMissSpectra = static_cast<unsigned int>(whistleMissTime * static_cast<float>(sampleRate) / static_cast<float>(windowSize) + 0.5f);
}

void UNSWDetector::WhistleState::interrogate(const std::vector<std::complex<float>>& complexSpectrum)
{
  // Find mean and standard deviation of the absolute values of the spectrum.
  std::vector<float> spectrum(complexSpectrum.size());
  float spectrumMean = 0.f;
  for (unsigned int i = 0; i < spectrum.size(); i++)
  {
    spectrum[i] = std::abs(complexSpectrum[i]);
    spectrumMean += spectrum[i];
  }
  spectrumMean /= static_cast<float>(spectrum.size());
  float spectrumStDev = 0.f;
  for (unsigned int i = 0; i < spectrum.size(); i++)
  {
    spectrumStDev += (spectrum[i] - spectrumMean) * (spectrum[i] - spectrumMean);
  }
  spectrumStDev = std::sqrt(spectrumStDev / static_cast<float>(spectrum.size()));
  // Find the threshold which must be surpassed by the sum of the amplitudes in the whistle band.
  float whistleThreshold;
  if (use2016Version)
  {
    whistleThreshold = spectrumMean + spectrumThreshold * spectrumStDev;
  }
  else
  {
    // Add current measurements to history (needed for 2015 detector).
    statsMemory.push_back({ spectrumMean, spectrumStDev });
    if (statsMemory.size() > statsRemember)
    {
      statsMemory.pop_front();
    }
    else
    {
      return;
    }
    // Find median of means and standard deviations of history.
    std::vector<float> means(statsRemember), devs(statsRemember);
    for (unsigned int i = 0; i < statsRemember; i++)
    {
      means[i] = statsMemory[i].mean;
      devs[i] = statsMemory[i].stDev;
    }
    std::sort(means.begin(), means.end());
    std::sort(devs.begin(), devs.end());
    const float lastSecondMean = (statsRemember % 2) ? means[statsRemember / 2] : (
      (means[statsRemember / 2 - 1] + means[statsRemember / 2]) * 0.5f);
    const float lastSecondStDev = (statsRemember % 2) ? devs[statsRemember / 2] : (
      (devs[statsRemember / 2 - 1] + devs[statsRemember / 2]) * 0.5f);
    whistleThreshold = std::max(spectrumMean + spectrumThreshold * spectrumStDev,
    lastSecondMean + temporalMediansThreshold * lastSecondStDev);
  }
  // Grow background zones which are discarded from the whistle band.
  unsigned int begin = spectrumWhistleBegin;
  unsigned int end = spectrumWhistleEnd;
  const float backgroundGrowthThreshold = spectrumMean + backgroundThreshold * spectrumStDev;
  const unsigned int numBuckets = 10;
  unsigned int growSize = (end - begin) / numBuckets;
  for (unsigned int i = 0; i < numBuckets; i++)
  {
    float bucketMean = 0.f;
    assert(begin + growSize <= spectrum.size());
    for (unsigned int j = begin; j < begin + growSize; j++)
    {
      bucketMean += spectrum[j];
    }
    bucketMean /= static_cast<float>(growSize);
    if (bucketMean < backgroundGrowthThreshold)
    {
      begin += growSize;
    }
    else
    {
      break;
    }
  }
  for (unsigned int i = 0; i < numBuckets; i++)
  {
    float bucketMean = 0.f;
    assert(end >= growSize);
    for (unsigned int j = end - growSize; j < end; j++)
    {
      bucketMean += spectrum[j];
    }
    bucketMean /= static_cast<float>(growSize);
    if (bucketMean < backgroundGrowthThreshold)
    {
      end -= growSize;
    }
    else
    {
      break;
    }
  }
  // Determine whether a whistle is found in this buffer.
  float found = false;
  if (begin < end)
  {
    float filteredMean = 0.f;
    for (unsigned int i = begin; i < end; i++)
    {
      filteredMean += spectrum[i];
    }
    filteredMean /= static_cast<float>(end - begin);
    found = filteredMean > whistleThreshold;
  }
  // Integrate this whistle measurement into the history and decide whether an action should be taken.
  if (whistleDone)
  {
    if (!found)
    {
      whistleMissCounter++;
      if (whistleMissCounter > nWhistleMissSpectra)
      {
        reset();
      }
    }
  }
  else
  {
    if (found)
    {
      if (whistleCounter == 0)
      {
        counterWhenWhistleStarted = currentCounter;
      }
      whistleCounter++;
      whistleMissCounter = 0;
    }
    else if (whistleCounter > 0)
    {
      whistleMissCounter++;
      if (whistleMissCounter > nWhistleMissSpectra)
      {
        reset();
      }
    }
    if (whistleCounter >= nWhistleOkaySpectra)
    {
      reset();
      whistleDone = true;
    }
  }
  currentCounter++;
}

void UNSWDetector::WhistleState::reset()
{
  whistleCounter = 0;
  whistleMissCounter = 0;
  whistleDone = false;
}
