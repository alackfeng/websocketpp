
#ifndef CHUNCKCODEC_PLUGIN_H
#define CHUNCKCODEC_PLUGIN_H

#include <map>
#include <string>
// #define NDEBUG
#include <assert.h>
#include "demangle.hpp"

using namespace std;

namespace sdk {
namespace plugins {
namespace base {

  class options;
  class plugin_manage;
  plugin_manage& plugins(); // plugins manage singleton

  class abstract_plugin
  {
  public:
    enum state
    {
      registered,
      initialized,
      started,
      stopped
    };

    virtual ~abstract_plugin() {}
    virtual state get_state()const = 0;
    virtual const string& name()const = 0;
    virtual void initialize(const options* ) = 0;
    virtual void startup() = 0;
    virtual void shutdown() = 0;
  };

  template<typename Impl>
  class plugin: public abstract_plugin
  {
  public:
    plugin(): name_(demangle(typeid(Impl).name())) {}
    virtual ~plugin() {}

    virtual state get_state()const override { return state_; }
    virtual const string& name()const override { return name_; }

    virtual void initialize(const options* params) override {
      if(state_ == registered) {
        state_ = initialized;
        static_cast<Impl*>(this)->plugin_initialize(params);
      }
      assert(state_ == initialized);
    }
    virtual void startup() override {
      if(state_ == initialized) {
        state_ = started;
        static_cast<Impl*>(this)->plugin_startup();
      }
      assert(state_ == started);
    }
    virtual void shutdown() override {
      if(state_ == started) {
        state_ = stopped;
        static_cast<Impl*>(this)->plugin_shutdown();
      }
      assert(state_ == stopped);
    }

  protected:
    plugin(const string& name): name_(name) {}
  private:
    state   state_ = abstract_plugin::registered;
    string  name_;
  };

} ///namespace base
} /// namespace plugins
} /// namespace sdk

#endif