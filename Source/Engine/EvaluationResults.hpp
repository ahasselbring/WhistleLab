/**
 * @file EvaluationResults.hpp declares the EvaluationResults class
 */

#pragma once

#include <QMetaType>


/**
 * @class EvaluationResults collects results of the evaluation of a detector
 */
class EvaluationResults final
{
public:
  /**
   * @brief EvaluationResults initializes members
   */
  EvaluationResults();
  /// the number of labeled whistles
  unsigned int positives = 0;
  /// the number of labeled whistles that have been hit
  unsigned int truePositives = 0;
  /// the number of detections that could not be matched with a labeled whistle
  unsigned int falsePositives = 0;
  /// the maximum delay from begin of the label to first report
  float maximumDelay = 0.f;
  /// the minimum delay from begin of the label to first report
  float minimumDelay = 0.f;
  /// the average delay from begin of the label to first report
  float averageDelay = 0.f;
  /// the maximum execution time that is needed to process 1s of audio data
  float maximumExecutionTimePerTime = 0.f;
  /// the minimum execution time that is needed to process 1s of audio data
  float minimumExecutionTimePerTime = 0.f;
  /// the average execution time that is needed to process 1s of audio data
  float averageExecutionTimePerTime = 0.f;
};

Q_DECLARE_METATYPE(EvaluationResults)
