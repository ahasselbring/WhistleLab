/**
 * @file UNSWDetector.hpp declares a detector class
 */

#pragma once

#include <complex>
#include <deque>

#include <fftw3.h>

#include "WhistleDetector.hpp"


/**
 * @class UNSWDetector is the whistle detector of the SPL team UNSW Sydney/Australia
 */
class UNSWDetector : public WhistleDetector<UNSWDetector>
{
public:
  /**
   * @brief UNSWDetector initializes members
   */
  UNSWDetector();
  /**
   * @brief evaluate evaluates the UNSWDetector on a given file
   * @param eh delivers and collects data for the evaluation
   */
  void evaluate(EvaluationHandle& eh) override;
private:
  /**
   * @struct WhistleState contains the current state of the whistle detection
   */
  struct WhistleState
  {
    /**
     * @struct MeanAndStDev stores a mean and estimated standard deviation
     */
    struct MeanAndStDev
    {
      /// the mean
      float mean;
      /// the standard deviation
      float stDev;
    };
    /**
     * @brief WhistleState initializes members
     */
    WhistleState();
    /**
     * @brief setSampleRate updates parameters that depend on the sample rate
     * @param sampleRate the new sample rate
     */
    void setSampleRate(const unsigned int sampleRate);
    /**
     * @brief interrogate checks for the whistle in one spectrum and integrates it into the state
     * @param complexSpectrum the spectrum of the signal that is currently processed
     */
    void interrogate(const std::vector<std::complex<float>>& complexSpectrum);
    /**
     * @brief reset resets the state
     */
    void reset();
    /// the number of spectra in which a whistle has been found
    unsigned int whistleCounter;
    /// the number of spectra in which a whistle has not been found (when it has been previously found)
    unsigned int whistleMissCounter;
    /// whether the whistle is finally accepted
    bool whistleDone;
    /// the counter value when the current whistle started
    unsigned int counterWhenWhistleStarted;
    /// a counter value that serves as timestamp
    unsigned int currentCounter;
    /// queue of means and standard deviations of spectra over a few frames
    std::deque<MeanAndStDev> statsMemory;
    /// the number of statistics to keep
    unsigned int statsRemember;
    /// the index in the spectrum at which the whistle band begins
    unsigned int spectrumWhistleBegin;
    /// the index in the spectrum at which the whistle band ends
    unsigned int spectrumWhistleEnd;
    /// the number of spectra that have to be accepted to report a whistle
    unsigned int nWhistleOkaySpectra;
    /// the number of spectra that may not be accepted before clearing the accepted spectra counter
    unsigned int nWhistleMissSpectra;
  };
  /// the lowest whistle band frequency
  static constexpr unsigned int fWhistleBegin = 2000;
  /// the highest whistle band frequency
  static constexpr unsigned int fWhistleEnd = 4000;
  /// factor of the standard deviation for determining the background growth threshold
  static constexpr float backgroundThreshold = 0.7f;
  /// factor of the standard deviation for determining the whistle threshold
  static constexpr float spectrumThreshold = 2.5f;
  /// factor of the standard deviation for determining the whistle threshold in the 2015 version
  static constexpr float temporalMediansThreshold = 5.0f;
  /// the duration for which the spectra have to be accepted to report a whistle [seconds]
  static constexpr float whistleOkayTime = 0.25f;
  /// the duration for which the spectra may not be accepted before clearing the accepted spectra counter [seconds]
  static constexpr float whistleMissTime = 0.083f;
  /// the window size of the DFT
  static constexpr unsigned int windowSize = 1024;
  /// decides whether the 2015 or 2016 version should be used
  static constexpr bool use2016Version = false;
  /// the current state of the whistle detection
  WhistleState state;
  /// the samples of the audio signal that are currently processed
  std::vector<float> window;
  /// the DFT of the samples contained in the current window
  std::vector<std::complex<float>> spectrum;
  /// a plan for FFTW for the FFT
  fftwf_plan fftPlan;
};
