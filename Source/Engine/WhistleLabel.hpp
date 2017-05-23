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
   * @brief WhistleLabel initializes members
   * @param start the first sample belonging to the whistle
   * @param end the first sample not belonging to the whistle anymore
   */
  WhistleLabel(const int start, const int end);
  /**
   * @brief read deserializes the object
   * @param object the JSON object from which the object is deserialized
   */
  void read(const QJsonObject& object);
  /**
   * @brief write serializes the object
   * @param object the JSON object to which the serialization is written
   */
  void write(QJsonObject& object);
  /// the first sample belonging to the whistle
  int start;
  /// the first sample not belonging to the whistle anymore
  int end;
};
