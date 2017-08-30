/**
 * @file AHDetector.hpp declares a detector class
 */

#pragma once

#include <array>
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
  /// the number of features that are available for the classifier
  static constexpr unsigned int numOfFeatures = 4;
  /**
   * @brief classify determines whether there is a whistle by some features
   * @param features the features that are available to the classifier
   * @return whether there is a whistle in the features
   */
  bool classify(const std::array<double, numOfFeatures>& features) const;
  /// the buffer size (a parameter)
  static constexpr unsigned int bufferSize = 4096;
  /// the minimum frequency of the whistle band (a parameter)
  static constexpr double minFrequency = 2000;
  /// the maximum frequency of the whistle band (a parameter)
  static constexpr double maxFrequency = 5000;
  /// the minimum power for the fundamental whistle frequency
  static constexpr double minRequiredPower = 117965;
  /// the minimum factor the power may be below the maximum power before ending boundary search
  static constexpr double minPowerOverMaxPower = 0.01;
  /// a buffer for the real input of the FFT
  std::vector<double> realBuffer;
  /// a buffer for the complex output of the FFT
  std::vector<std::complex<double>> complexBuffer;
  /// a buffer for the power at each frequency
  std::vector<double> powerBuffer;
  /// a plan for FFTW for the FFT
  fftw_plan fftPlan;
  /// whether the detector is in training mode
  bool training;
};
