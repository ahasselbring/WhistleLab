/**
 * @file AHDetector.hpp declares a detector class
 */

#pragma once

#include <complex>

#include <fftw3.h>

#include "WhistleDetector.hpp"


/**
 * @class AHDetector is one of the whistle detectors developed for this thesis
 */
class AHDetector : public WhistleDetector<AHDetector>
{
public:
  /**
   * @brief AHDetector initializes members and FFTW plan
   */
  AHDetector();
  /**
   * @brief ~AHDetector destroys FFTW plan
   */
  ~AHDetector();
  /**
   * @brief evaluate evaluates the AHDetector on a given file
   * @param eh delivers and collects data for the evaluation
   */
  void evaluate(EvaluationHandle& eh) override;
  /**
   * @brief trainOnDatabase trains the AHDetector on a given database
   * @param db the database on which the detector is trained
   */
  void trainOnDatabase(const SampleDatabase& db) override;
private:
  /// the buffer size (a parameter)
  static constexpr unsigned int bufferSize = 4096;
  /// the minimum frequency of the whistle band (a parameter)
  static constexpr double minFrequency = 2000;
  /// the maximum frequency of the whistle band (a parameter)
  static constexpr double maxFrequency = 5000;
  /// the minimum power for the fundamental whistle frequency
  static constexpr double minRequiredPower = 3000;
  /// the minimum ratio of the fundamental whistle band over the following non-whistle band
  static constexpr double whistleBand0OverStopBand0 = 15;
  /// the minimum ratio of the second harmonic whistle band over the previous non-whistle band
  static constexpr double whistleBand1OverStopBand0 = 1.5;
  /// the minimum ratio of both whistle bands over the non-whistle bands
  static constexpr double whistleBandOverStopBand = 2;
  /// a buffer for the real input of the FFT
  std::vector<double> realBuffer;
  /// a buffer for the complex output of the FFT
  std::vector<std::complex<double>> complexBuffer;
  /// a buffer for the power at each frequency
  std::vector<double> powerBuffer;
  /// a plan for FFTW for the FFT
  fftw_plan fftPlan;
};
