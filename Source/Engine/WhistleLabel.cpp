/**
 * @file WhistleLabel.cpp implements methods for the whistle label
 */

#include <algorithm>

#include <QJsonObject>

#include "WhistleLabel.hpp"


void WhistleLabel::read(const QJsonObject& object)
{
  start = std::max(0, object["start"].toInt());
  end = std::max(0, object["end"].toInt());
}

void WhistleLabel::write(QJsonObject& object) const
{
  object["start"] = start;
  object["end"] = end;
}
