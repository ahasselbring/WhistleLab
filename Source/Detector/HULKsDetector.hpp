/**
 * @file HULKsDetector.hpp declares a detector class
 */

#pragma once

#include <complex>

#include <fftw3.h>

#include "WhistleDetector.hpp"


/**
 * @class HULKsDetector is the whistle detector of the SPL team HULKs
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
   * @brief evaluate evaluates the HULKsDetector on a given file
   * @param eh delivers and collects data for the evaluation
   */
  void evaluate(EvaluationHandle& eh) override;
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
