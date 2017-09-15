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
  /// the average delay from begin of the label to first report
  float averageDelay = 0.f;
};

Q_DECLARE_METATYPE(EvaluationResults)