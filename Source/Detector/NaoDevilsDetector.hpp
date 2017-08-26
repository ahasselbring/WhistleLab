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
   * @brief evaluate evaluates the NaoDevilsDetector on a given file
   * @param eh delivers and collects data for the evaluation
   */
  void evaluate(EvaluationHandle& eh) override;
private:
  /// the FFT window size (a parameter)
  static constexpr unsigned int windowSize = 1024;
  /// the minimum frequency of the fundamental frequency (a parameter)
  static constexpr unsigned int minFrequency = 2000;
  /// the maximum frequency of the fundamental frequency (a parameter)
  static constexpr unsigned int maxFrequency = 4000;
  /// the minimum amplitude of the fundamental frequency (a parameter)
  static constexpr float minAmp = 35.f;
  /// the minimum factor between fundamental frequency and first overtone frequency (a parameter)
  static constexpr float overtoneMultMin1 = 1.8f;
  /// the maximum factor between fundamental frequency and first overtone frequency (a parameter)
  static constexpr float overtoneMultMax1 = 2.2f;
  /// the minimum amplitude of the first overtone (a parameter)
  static constexpr float overtoneMinAmp1 = 2.0f;
  /// the minimum factor between fundamental frequency and second overtone frequency (a parameter)
  static constexpr float overtoneMultMin2 = 2.8f;
  /// the maximum factor between fundamental frequency and second overtone frequency (a parameter)
  static constexpr float overtoneMultMax2 = 3.2f;
  /// the minimum amplitude of the second overtone (a parameter)
  static constexpr float overtoneMinAmp2 = 2.0f;
  /// the number of buffers that are classified positive after a whistle has been detected (a parameter)
  static constexpr unsigned int release = 4;
  /// the number of successive buffers that have to be classified positive to be accepted as whistle (a parameter)
  static constexpr unsigned int attack = 4;
  /// whether Hann windowing should be used before executing the FFT (a parameter)
  static constexpr bool useHannWindowing = true;
  /// the number of amplitudes coming out of the FFT (derived parameter)
  static constexpr unsigned int ampSize = windowSize / 2 + 1;
  /// a buffer for the real input of the FFT
  std::vector<float> realBuffer;
  /// a buffer for the complex output of the FFT
  std::vector<std::complex<float>> complexBuffer;
  /// a plan for FFTW for the FFT
  fftwf_plan fftPlan;
};
