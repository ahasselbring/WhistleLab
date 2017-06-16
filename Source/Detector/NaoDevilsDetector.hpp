/**
 * @file NaoDevilsDetector.hpp declares a detector class
 */

#pragma once

#include <complex>

#include <fftw3.h>

#include "WhistleDetector.hpp"


/**
 * @class NaoDevilsDetector is the whistle detector of the SPL team Nao Devils Dortmund
 */
class NaoDevilsDetector : public WhistleDetector<NaoDevilsDetector>
{
public:
  /**
   * @brief NaoDevilsDetector initializes members and FFTW plan
   */
  NaoDevilsDetector();
  /**
   * @brief ~NaoDevilsDetector destroys FFTW plan
   */
  ~NaoDevilsDetector();
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
  /// the FFT window size (a parameter)
  static constexpr unsigned int windowSize = 1024;
  /// the minimum frequency of the fundamental frequency (a parameter)
  static constexpr double minFrequency = 2000.0;
  /// the maximum frequency of the fundamental frequency (a parameter)
  static constexpr double maxFrequency = 4000.0;
  /// the minimum amplitude of the fundamental frequency (a parameter)
  static constexpr double minAmp = 35.0;
  /// the minimum factor between fundamental frequency and first overtone frequency (a parameter)
  static constexpr double overtoneMultMin1 = 1.8;
  /// the maximum factor between fundamental frequency and first overtone frequency (a parameter)
  static constexpr double overtoneMultMax1 = 2.2;
  /// the minimum amplitude of the first overtone (a parameter)
  static constexpr double overtoneMinAmp1 = 2.0;
  /// the minimum factor between fundamental frequency and second overtone frequency (a parameter)
  static constexpr double overtoneMultMin2 = 2.8;
  /// the maximum factor between fundamental frequency and second overtone frequency (a parameter)
  static constexpr double overtoneMultMax2 = 3.2;
  /// the minimum amplitude of the second overtone (a parameter)
  static constexpr double overtoneMinAmp2 = 2.0;
  /// whether Hann windowing should be used before executing the FFT (a parameter)
  static constexpr bool useHannWindowing = true;
  /// the number of amplitudes coming out of the FFT (derived parameter)
  static constexpr unsigned int ampSize = windowSize / 2 + 1;
  /// a buffer for the real input of the FFT
  std::vector<double> realBuffer;
  /// a buffer for the complex output of the FFT
  std::vector<std::complex<double>> complexBuffer;
  /// a plan for FFTW for the FFT
  fftw_plan fftPlan;
};
