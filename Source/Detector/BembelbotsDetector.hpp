/**
 * @file BembelbotsDetector.hpp declares a detector class
 */

#pragma once

#include <complex>

#include <fftw3.h>

#include "WhistleDetector.hpp"


/**
 * @class BembelbotsDetector is the whistle detector of the SPL team Bembelbots
 */
class BembelbotsDetector : public WhistleDetector<BembelbotsDetector>
{
public:
  /**
   * @brief BembelbotsDetector initializes members
   */
  BembelbotsDetector();
  /**
   * @brief evaluate evaluates the BembelbotsDetector on a given file
   * @param eh delivers and collects data for the evaluation
   */
  void evaluate(EvaluationHandle& eh) override;
private:
  /**
   * @struct WhistleMatch contains the current state of the whistle detection
   */
  struct WhistleMatch
  {
    /// whether a match is available
    bool available;
    /// the length of the current match in milliseconds
    unsigned int lengthMs;
    /// the maximal volume of the current match in dB
    float maxVolumeDb;
  };
  /// the length of a buffer in milliseconds
  static constexpr unsigned int bufferSizeMs = 50;
  /// the frequency above which the peak has to be for a whistle in Hertz
  static constexpr unsigned int thresholdHz = 2000;
  /// the volume above which a whistle must be at least once in dB
  static constexpr int volumeThresholdDb = -20;
  /// the minimal length of a whistle to be accepted in milliseconds
  static constexpr unsigned int minSignalLengthMs = 400;
  /// a parameter for smoothing of the spectrum
  static constexpr unsigned int filterStrength = 3;
  /// the samples of the audio signal that are currently processed
  std::vector<float> audioContainer;
  /// the DFT of the samples contained in audioContainer
  std::vector<std::complex<float>> spectrum;
  /// the smoothed absolute values of the spectrum
  std::vector<float> smoothedSpectrum;
  /// the current state of the whistle detection
  WhistleMatch match;
  /// a plan for FFTW for the FFT
  fftwf_plan fftPlan;
};
