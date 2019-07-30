#ifndef DEMANGLE_HPP
#define DEMANGLE_HPP

#include <string>
#include <typeinfo>

namespace sdk {
namespace plugins {
namespace base {

std::string demangle(const char* name);

template <class T>
std::string type(const T& t) {

    return demangle(typeid(t).name());
}

} ///namespace base
} /// namespace plugins
} /// namespace sdk

#endif