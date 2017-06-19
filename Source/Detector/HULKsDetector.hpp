/**
 * @file HULKsDetector.hpp declares a detector class
 */

#pragma once

#include <complex>

#include <fftw3.h>

#include "WhistleDetector.hpp"


/**
 * @brief HULKsDetector is the whistle detector of the SPL team HULKs
 */
class HULKsDetector : public WhistleDetector<HULKsDetector>
{
public:
  /**
   * @brief HULKsDetector initializes members and FFTW plan
   */
  HULKsDetector();
  /**
   * @brief ~HULKsDetector destroys FFTW plan
   */
  ~HULKsDetector();
  /**
   * @brief getPreferredBufferSize returns the number of samples that the detector wants to get
   * @return the number of samples that the detector wants to get
   */
  unsigned int getPreferredBufferSize() const override;
  /**
   * @brief classify should determine whether a short audio snippet contains a whistle sound
   * @param samples a single channel vector of audio samples that may or may not contain a whistle sound
   * @param sampleRate the sample rate in samples per second
   * @return true iff the given audio sample contains a whistle
   */
  bool classify(const std::vector<float>& samples, const unsigned int sampleRate) override;
private:
  /// the buffer size (a parameter)
  static constexpr unsigned int bufferSize = 8192;
  /// the minimum frequency of the whistle band (a parameter)
  static constexpr double minFrequency = 2000;
  /// the maximum frequency of the whistle band (a parameter)
  static constexpr double maxFrequency = 4000;
  /// the threshold for whistle power over stop band power (a parameter)
  static constexpr double threshold = 500;
  /// a buffer for the real input of the FFT
  std::vector<double> realBuffer;
  /// a buffer for the complex output of the FFT
  std::vector<std::complex<double>> complexBuffer;
  /// a plan for FFTW for the FFT
  fftw_plan fftPlan;
};
