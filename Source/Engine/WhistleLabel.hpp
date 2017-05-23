/**
 * @file WhistleLabel.hpp declares the whistle label class
 */

#pragma once


class QJsonObject;

/**
 * @class WhistleLabel a range of samples where a whistle starts and ends
 */
class WhistleLabel final
{
public:
  /**
   * @brief read deserializes the object
   * @param object the JSON object from which the object is deserialized
   */
  void read(const QJsonObject& object);
  /**
   * @brief write serializes the object
   * @param object the JSON object to which the serialization is written
   */
  void write(QJsonObject& object) const;
  /// the first sample belonging to the whistle
  int start = 0;
  /// the first sample not belonging to the whistle anymore
  int end = 0;
};
