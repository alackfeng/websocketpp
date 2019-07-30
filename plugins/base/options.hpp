#ifndef CHUNKCC_PLUGIN_OPTIONS
#define CHUNKCC_PLUGIN_OPTIONS

#ifdef __has_include
#if __has_include("any.hpp")
#  include "any.hpp"
#  define have_any 1
#elif __has_include(<any>)
#  include <any>
#  define have_any 1
#else
#  define have_any 0
#  error "Missing <any>"
#endif
#else
#  include "any.hpp"
// #  error "Missing __has_include"
#endif

#define THROW(begin, itr, end) \
std::string err(begin); \
err += itr->first; \
err += end; \
throw std::runtime_error(err.c_str());

namespace sdk {
namespace plugins {
namespace base {

  class options
  {
  public:
    options() {}
    ~options() {
      std::map<std::string, std::any>::iterator itr = opts.begin();
      for(; itr != opts.end();) {
        auto& val = itr->second;
        printf("opitons.~options call. %s\n", val.type().name());
        if(val.type() == typeid(int*)) { delete std::any_cast<int*>(val); } 
        else if(val.type() == typeid(char*)) { delete std::any_cast<char*>(val); }
        opts.erase(itr++);
      }
    }

    // 定义参数KEY字段
    static const char* KEY_FILENAME;
    static const char* KEY_LOGNAME;

    template<typename T>
    void Set(const std::string& key, T value) {

      if(typeid(T) == typeid(int)
        || typeid(T) == typeid(long)
        || typeid(T) == typeid(double)
        || typeid(T) == typeid(float)
        || typeid(T) == typeid(int64)
        || typeid(T) == typeid(int8)
        || typeid(T) == typeid(int32)
        || typeid(T) == typeid(char)
        ) {
        opts[key] = std::any(value);
      }
      else {
        auto val = std::any(value);
        std:string err(key);
        err += ", Set type ";
        err += val.type().name();
        throw std::runtime_error(err.c_str());
      }
    }

    template<typename T, int Len>
    void SetP(const std::string& key, T* value) {

      if(typeid(T) == typeid(int)
        || typeid(T) == typeid(char)) {
        T* uPtr = new T[Len];
        memset(uPtr, 0, Len);
        memcpy(uPtr, value, Len);
        opts[key] = std::any(uPtr);
      }
      else {
        auto val = std::any(value);
        std:string err(key);
        err += ", SetP type ";
        err += val.type().name();
        throw std::runtime_error(err.c_str());
      }
    }
    template<typename T>
    T Get(const std::string& key) {
      auto itr = opts.find(key);
      if(itr == opts.end()) {
        THROW("first Set ", itr, " option");
      }

      try {
        auto ret = 0;
        auto& val = itr->second;
        std::cout << "options.Get KEY " << itr->first << ", Value " << val.type().name() << ": ";
        if(val.type() == typeid(int)
          || val.type() == typeid(long)
          || val.type() == typeid(double)
          || val.type() == typeid(float)
          || val.type() == typeid(int64)
          || val.type() == typeid(int8)
          || val.type() == typeid(int32)
          || val.type() == typeid(char)
          ) {
          return static_cast<T>(std::any_cast<T>(val));
        }
        else {
          std:string err(itr->first);
          err += ", type ";
          err += val.type().name();
          throw std::runtime_error(err.c_str());
        }
        return 0;

      } catch(const std::bad_any_cast& e) {
        std::cerr << "options::Get: bad_any_cast error, " << e.what() << std::endl;
      } catch(const std::exception& e) {
        std::cerr << "options::Get: exception error, " << e.what() << '\n';
      }
      return 0;
    }

    template<typename T>
    T* GetP(const std::string& key) {
      auto itr = opts.find(key);
      if(itr == opts.end()) {
        THROW("first SetP ", itr, " option");
      }

      try {
        auto& val = itr->second;
        std::cout << "options.GetP KEY " << itr->first << ", Value " << val.type().name() << ": ";
        if(val.type() == typeid(int*)
          || val.type() == typeid(char*)
          ) {
          std::cout << "V " << std::any_cast<T*>(val) << "." << std::endl;
          return (std::any_cast<T*>(val));
        }
        else {
          std:string err(itr->first);
          err += ", type ";
          err += val.type().name();
          throw std::runtime_error(err.c_str());
        }
        return 0;

      } catch(const std::bad_any_cast& e) {
        std::cerr << "options::GetP: bad_any_cast error, " << e.what() << std::endl;
      } catch(const std::exception& e) {
        std::cerr << "options::GetP: exception error, " << e.what() << '\n';
      }
      return 0;
    }

    
  private:
    std::map<std::string, std::any> opts;
  };

  // 定义参数KEY字段
  const char* options::KEY_FILENAME   = "filename";
  const char* options::KEY_LOGNAME    = "logname";

} ///namespace base
} /// namespace plugins
} /// namespace sdk

#endif