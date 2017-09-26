/**
 * @file AHDetector.hpp declares a detector class
 */

#pragma once

#include <array>
#include <complex>

#include <fann.h>

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
  static constexpr unsigned int numOfFeatures = 6;
  typedef std::array<double, numOfFeatures> FeatureVector;
  /**
   * @struct TrainingExample contains pairs of input and desired output of the classifier
   */
  struct TrainingExample
  {
    /// the input of the classifier
    FeatureVector input;
    /// the desired output of the classifier
    bool output;
  };
  /**
   * @brief classifyJ48 determines whether there is a whistle by some features (in this case by a J48-trained decision tree)
   * @param features the features that are available to the classifier
   * @return whether there is a whistle in the features
   */
  bool classifyJ48(const FeatureVector& features) const;
  /**
   * @brief classifyNN determines whether there is a whistle by some features (in this case an artificial neural network)
   * @param features the features that are available to the classifier
   * @return whether there is a whistle in the features
   */
  bool classifyNN(const FeatureVector& features) const;
  /**
   * @brief trainJ48 trains a decision tree
   */
  void trainJ48();
  /**
   * @brief trainNN trains a neural network
   */
  void trainNN();
  /// whether the artificial neural network should be used for classification (instead of the decision tree)
  static constexpr bool useNN = true;
  /// the buffer size (a parameter)
  static constexpr unsigned int bufferSize = 2048;
  /// the minimum frequency of the whistle band (a parameter)
  static constexpr double minFrequency = 2000;
  /// the maximum frequency of the whistle band (a parameter)
  static constexpr double maxFrequency = 4000;
  /// the minimum amplitude for the fundamental whistle frequency
  static constexpr double minRequiredAmplitude = 0.05;
  /// the minimum factor the amplitude may be below the maximum amplitude before ending boundary search
  static constexpr double minAmplitudeOverMaxAmplitude = 0.01;
  /// a buffer for the real input of the FFT
  std::vector<double> realBuffer;
  /// a buffer for the complex output of the FFT
  std::vector<std::complex<double>> complexBuffer;
  /// a buffer for the amplitude at each frequency
  std::vector<double> amplitudeBuffer;
  /// a plan for FFTW for the FFT
  fftw_plan fftPlan;
  /// whether the detector is in training mode
  bool training;
  /// the collected data during training
  std::vector<TrainingExample> trainingExamples;
  /// the neural network (only if the neural network classifier should be used)
  fann* ann;
  /// the means of the features
  FeatureVector means;
  /// the standard deviations of the features
  FeatureVector stddevs;
};
