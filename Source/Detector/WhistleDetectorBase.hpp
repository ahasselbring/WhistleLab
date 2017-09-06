/**
 * @file WhistleDetectorBase.hpp declares an interface for whistle detectors
 */

#pragma once

#include <vector>

#include "Engine/EvaluationResults.hpp"
#include "Engine/SampleDatabase.hpp"

#include "EvaluationHandle.hpp"


/**
 * @class WhistleDetectorBase is an interface for single channel whistle detectors
 */
class WhistleDetectorBase
{
public:
  /**
   * @brief ~WhistleDetectorBase virtual destructor for polymorphism
   */
  virtual ~WhistleDetectorBase() = default;
  /**
   * @brief evaluate evaluates a detector on a given file
   * @param eh delivers and collects data for the evaluation
   */
  virtual void evaluate(EvaluationHandle& eh) = 0;
  /**
   * @brief trainOnDatabase trains a detector on a given database
   * @param db the database on which the detector is trained
   */
  virtual void trainOnDatabase(const SampleDatabase& db);
  /**
   * @brief evaluateOnDatabase evaluates a detector on a given database
   * @param db the database on which the detector is evaluated
   * @param results is filled with the results of the evaluation
   */
  virtual void evaluateOnDatabase(const SampleDatabase& db, EvaluationResults* results = nullptr);
};
