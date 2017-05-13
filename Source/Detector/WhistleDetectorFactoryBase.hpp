/**
 * @file WhistleDetectorFactoryBase.hpp declares a base class for whistle detector factories
 */

#pragma once

#include <memory>
#include <string>
#include <typeindex>


class WhistleDetectorBase;

/**
 * @class WhistleDetectorFactoryBase is a base class for whistle detector factories
 */
class WhistleDetectorFactoryBase
{
public:
  /**
   * @brief WhistleDetectorFactoryBase inserts the factory into the list of factories
   */
  WhistleDetectorFactoryBase(const std::type_index& type);
  /**
   * @brief ~WhistleDetectorFactoryBase virtual destructor for polymorphism
   */
  virtual ~WhistleDetectorFactoryBase() = default;
  /**
   * @brief make creates an instance of the detector
   * @return a shared pointer to the newly created detector instance
   */
  virtual std::shared_ptr<WhistleDetectorBase> make() const = 0;
  /**
   * @brief make creates an instance of a detector with a given name
   * @param name the name of the detector class
   * @return a shared pointer to the newly created detector instance
   */
  static std::shared_ptr<WhistleDetectorBase> make(const std::string& name);
  /**
   * @brief use is called from somewhere to force the construction of a factory
   */
  void use() const;
private:
  /**
   * @brief demangle demangles C++ typenames
   * @param type a typename as returned by typeid(T).name()
   * @return the demangled name
   */
  static std::string demangle(const char* type);
  /// the first factory in the list of factories
  static WhistleDetectorFactoryBase* first;
  /// the next factory in the list of factories
  WhistleDetectorFactoryBase* next;
  /// the name of the detector that can be created by this factory
  const std::string name;
};
