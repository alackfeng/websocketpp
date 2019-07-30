
#include "plugin_manage.hpp"
#include <vector>
// #include "../utils/bdlog.h"

namespace sdk {
namespace plugins {
namespace base {

  plugin_manage::plugin_manage()
  : version_(0), fullmd5_(0) {
    printf("plugin_manage::plugin_manage call.\n");
  }
  plugin_manage::~plugin_manage() {
    printf("plugin_manage::~plugin_manage over.\n");
    // log_close();
  }  

  void plugin_manage::set_version(int64 version) {
    version_ = version;
  }
  void plugin_manage::set_version(string version) {
    std::vector<int> ver;
    std::size_t pos = 0;
    std::size_t find = version.find('.', pos);
    while(find != std::string::npos) {
      string iV = version.substr(pos, find-pos);
      int iVer = std::stoi(iV);
      ver.push_back(iVer);
      // next
      pos = find + 1;
      find = version.find('.', pos);
    }
    // last
    int iVer = std::stoi(version.substr(pos));
    ver.push_back(iVer);

    version_ = (ver[0] & 0xFF) << 24 | (ver[1] & 0xFF) << 16 | (ver[2] & 0xFFFF);
  }

  const int64& plugin_manage::version()const {
    return version_;
  }
  const string& plugin_manage::version_string()const {
    static string ver;
    ver.clear();

    int iVer = version_ >> 24 & 0xFF;
    ver += std::to_string(iVer);
    ver += ".";

    iVer = version_ >> 16 & 0xFF;
    ver += std::to_string(iVer);
    ver += ".";

    iVer = version_  & 0xFFFF;
    ver += std::to_string(iVer);
    return ver;
  }

  void plugin_manage::set_fullmd5(int md5) {
    fullmd5_ = md5; 
  }
  const int& plugin_manage::fullmd5()const {
    return fullmd5_;
  }

  void plugin_manage::set_devmode(bool mode) {
    devmode_ = mode;
  }
  const bool& plugin_manage::devmode()const {
    return devmode_;
  }

  void plugin_manage::set_logging(const char* filename, const int& level) {

    printf("plugin_manage::set_logging log name <%d>%s\n", level, filename);
    // log_init(filename,  level ? level : LOG_LEVEL_ALL);
  }

  abstract_plugin* plugin_manage::find_plugin(const string& name)const {
    auto itr = plugins.find(name);
    if(itr == plugins.end()) {
      return nullptr;
    }
    return itr->second.get();
  }
  abstract_plugin& plugin_manage::get_plugin(const string& name)const {
    auto ptr = find_plugin(name);
    printf("plugin_manage::get_plugin name %s, %p\n", name.c_str(), ptr);
    assert(ptr != nullptr);
    return *ptr;
  }

  plugin_manage& plugin_manage::instance() {
    static plugin_manage plugin_mgr_;
    return plugin_mgr_;
  }

  plugin_manage& plugins() {
    return plugin_manage::instance();
  }


} ///namespace base
} /// namespace plugins
} /// namespace sdk
