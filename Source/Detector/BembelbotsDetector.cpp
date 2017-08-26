/**
 * @file BembelbotsDetector.cpp implements methods of the BembelbotsDetector class
 */

#include <algorithm>
#include <cmath>

#include "BembelbotsDetector.hpp"


BembelbotsDetector::BembelbotsDetector()
{
}

void BembelbotsDetector::evaluate(EvaluationHandle& eh)
{
  // Determine parameters that depend on the sample rate.
  const unsigned int bufferSize = eh.getSampleRate() * bufferSizeMs / 1000;
  const unsigned int dftSize = bufferSize / 2 + 1;
  // Clear member variables.
  match.available = false;
  audioContainer.resize(bufferSize);
  spectrum.resize(dftSize);
  smoothedSpectrum.resize((dftSize / filterStrength) + ((dftSize % filterStrength) ? 1 : 0));
  // Create FFTW plan for this sample rate.
  fftPlan = fftwf_plan_dft_r2c_1d(bufferSize, audioContainer.data(), reinterpret_cast<fftwf_complex*>(spectrum.data()), FFTW_ESTIMATE);
  while (eh.readSingleChannel(audioContainer.data(), bufferSize) == bufferSize)
  {
    // The abs is not present in original Bembelbots code, but I assume it is more correct with it.
    const float volDb = 20.f * std::log10(std::abs(*std::max_element(audioContainer.begin(), audioContainer.end(), [](const float a, const float b){ return std::abs(a) < std::abs(b); })));
    // Execute FFT.
    fftwf_execute(fftPlan);
    // Smoothen spectrum.
    for (unsigned int i = 0; i < dftSize / filterStrength; i++)
    {
      float sum = 0.f;
      for (unsigned int j = 0; j < filterStrength; j++)
      {
        sum += std::abs(spectrum[i * filterStrength + j]);
      }
      smoothedSpectrum[i] = sum / filterStrength;
    }
    if (dftSize % filterStrength)
    {
      float sum = 0.f;
      for (unsigned int j = 0; j < (dftSize % filterStrength); j++)
      {
        sum += std::abs(spectrum[spectrum.size() - j - 1]);
      }
      smoothedSpectrum[smoothedSpectrum.size() - 1] = sum / static_cast<float>(dftSize % filterStrength);
    }
    // Find the peak frequency (i.e. the frequency with highest amplitude) in the smoothed spectrum.
    unsigned int maxIndex = 0;
    for (unsigned i = 1; i < smoothedSpectrum.size(); i++)
    {
      if (smoothedSpectrum[i] > smoothedSpectrum[maxIndex])
      {
        maxIndex = i;
      }
    }
    const float peakHz = static_cast<float>(maxIndex * eh.getSampleRate() * filterStrength) / static_cast<float>(bufferSize);
    // Integrate into existing whistle or start a new detection.
    if (match.available)
    {
      if (peakHz < thresholdHz)
      {
        if (match.lengthMs > minSignalLengthMs && match.maxVolumeDb > volumeThresholdDb)
        {
          eh.report(-static_cast<int>(match.lengthMs * eh.getSampleRate() / 1000) / 2);
        }
        match.available = false;
      }
      else
      {
        match.lengthMs += bufferSizeMs;
        if (volDb > match.maxVolumeDb)
        {
          match.maxVolumeDb = volDb;
        }
      }
    }
    else if (peakHz >= thresholdHz)
    {
      match.available = true;
      match.lengthMs = bufferSizeMs;
      match.maxVolumeDb = volDb;
    }
  }
  fftwf_destroy_plan(fftPlan);
}
