
#include "exception.hpp"

namespace sdk {
namespace cores {
namespace utils {


std::map<int, std::string> err_messages = std::map<int, std::string>{
  {std::make_pair(FILE_OPEN_ERROR, "打开文件失败")},
  {std::make_pair(FILE_FDSOCK_ERROR, "file socket error")},

  // bigfile
  {BIGFILE_PTR_RELEASED, "bigfile已经释放了"},
  {BIGFILE_SIZE_FIXED, "bigfile文件大小固定不变"},
  {BIGFILE_VERSION_NOT_FIT, "无法识别存储体的版本信息"},
  {BIGFILE_NOT_EXIST, "bigfile不存在"},
  // bigfile::blocktarget
  {BLOCKTARGET_NO_IN_BIGFILE, "blocktarget不属于bigfile范围内"},
  {BLOCKTARGET_MAX_LIMIT, "超过了blocktarget最大限制容量"},

  // chunk

  // chunkmanager
  {CHUNKMANAGER_ID_NOT_FIX, "chunkmanager不存在bigfile"},
  {CHUNKMANAGER_META_BREAKDOWN, "chunkmanager元数据损坏！"},
};


char const* sdkexception::what() const throw() {
  if(m_msg != "") {
    return m_msg.c_str();
  }

  std::map<int, std::string>::iterator itr = err_messages.find(m_code);
  if(itr != err_messages.end()) {
    return itr->second.c_str();
  }
  return "Unknown";
}

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {
