
#ifndef PLUGIN_MANAGE_H
#define PLUGIN_MANAGE_H

#include <memory>
#include "plugin.hpp"
#include "Common.h"


namespace sdk {
namespace plugins {
namespace base {


  class plugin_manage
  {
  public:
    plugin_manage();
    ~plugin_manage();
    static plugin_manage& instance();

    // 全局配置参数

    void set_version(int64 version);
    void set_version(string version);
    const int64& version()const;
    const string& version_string()const;

    void set_fullmd5(int md5);
    const int& fullmd5()const;

    void set_devmode(bool mode);
    const bool& devmode()const;

    void set_logging(const char* filename, const int& level);


    abstract_plugin* find_plugin(const string& name)const;
    abstract_plugin& get_plugin(const string& name)const;

    template<typename Plugin, bool only = false, typename...Args>
    auto register_plugin(Args...args) {
      if (only) { // 只生成一份插件
        auto existing = find_plugin<Plugin>();
        if(existing) return existing;
      }

      auto plug = new Plugin(args...);
      const std::string name_ = only ? plug->name(): std::to_string((long)plug);
      plugins[name_].reset(plug);
      printf("register_plugin<%s>. %p %ld\n", name_.c_str(), plug, (long)plug);
      return plug;
    }

    template<typename Plugin>
    Plugin* find_plugin(const string& name = "")const {
      string name_ = demangle(typeid(Plugin).name());
      if(name != "")  name_ = name;
      printf("find_plugin<%s>.\n", name_.c_str());
      return dynamic_cast<Plugin*>(find_plugin(name_));
    }

  private:
    map<string, std::unique_ptr<abstract_plugin>> plugins;

    // 全局配置参数
    int64 version_;
    int fullmd5_; // 编码生成M片的时候记录M片的完整MD5
    bool devmode_; // 开发者模式
  };

} ///namespace base
} /// namespace plugins
} /// namespace sdk


#endif