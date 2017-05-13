/**
 * @file WhistleDetectorFactory.hpp contains a template class for detector factories
 */

#pragma once

#include <memory>

#include "WhistleDetectorFactoryBase.hpp"


/**
 * @class WhistleDetectorFactory implements the factory base for a specific detector
 */
template<typename Derived>
class WhistleDetectorFactory : public WhistleDetectorFactoryBase
{
public:
  /**
   * @brief WhistleDetectorFactory calls the constructor of the base class
   */
  WhistleDetectorFactory()
    : WhistleDetectorFactoryBase(typeid(Derived))
  {
  }
  /**
   * @brief make creates an instance of the detector
   * @return a shared pointer to the newly created detector instance
   */
  virtual std::shared_ptr<WhistleDetectorBase> make() const override
  {
    return std::make_shared<Derived>();
  }
};
