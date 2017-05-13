/**
 * @file WhistleDetector.hpp contains a template base class for detectors
 */

#pragma once

#include "WhistleDetectorBase.hpp"
#include "WhistleDetectorFactory.hpp"


/**
 * @class WhistleDetector adds a factory to the WhistleDetectorBase via CRTP
 */
template<typename Derived>
class WhistleDetector : public WhistleDetectorBase
{
public:
  /**
   * @brief WhistleDetector calls the use method of the factory to force the compiler to construct it
   */
  WhistleDetector()
  {
    factory.use();
  }
private:
  /// a factory that can create an instance of this detector
  static WhistleDetectorFactory<Derived> factory;
};

template<typename Derived>
WhistleDetectorFactory<Derived> WhistleDetector<Derived>::factory;
