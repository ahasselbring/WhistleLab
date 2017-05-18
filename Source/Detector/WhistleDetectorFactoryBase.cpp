/**
 * @file WhistleDetectorFactoryBase.cpp implements methods of the class WhistleDetectorFactoryBase
 */

#include <cstdlib>
#include <stdexcept>

#include <cxxabi.h>

#include "WhistleDetectorFactoryBase.hpp"


WhistleDetectorFactoryBase* WhistleDetectorFactoryBase::first = nullptr;

WhistleDetectorFactoryBase::WhistleDetectorFactoryBase(const std::type_index& type)
  : name(demangle(type.name()))
{
  next = first;
  first = this;
}

std::shared_ptr<WhistleDetectorBase> WhistleDetectorFactoryBase::make(const std::string& name)
{
  for (const WhistleDetectorFactoryBase* factory = first; factory != nullptr; factory = factory->next)
  {
    if (factory->name == name)
    {
      return factory->make();
    }
  }
  throw std::runtime_error("No factory could create a detector for a given name!");
}

std::vector<std::string> WhistleDetectorFactoryBase::getDetectorNames()
{
  std::vector<std::string> result;
  for (const WhistleDetectorFactoryBase* factory = first; factory != nullptr; factory = factory->next)
  {
    result.push_back(factory->name);
  }
  return result;
}

void WhistleDetectorFactoryBase::use() const
{
}

std::string WhistleDetectorFactoryBase::demangle(const char* type)
{
  size_t length = 0;
  int status = 0;
  char* demangledName = abi::__cxa_demangle(type, nullptr, &length, &status);
  if (status != 0 || demangledName == nullptr)
  {
    throw std::runtime_error("Name demangling failed!");
  }
  std::string result(demangledName);
  std::free(demangledName);
  return result;
}
