
#include "exception.hpp"

namespace sdk {
namespace cores {
namespace utils {


std::map<int, std::string> err_messages = std::map<int, std::string>{
  // base
  {STREAM_NOT_SUPPORT_NEGATIVE_NUMBER, "序列化暂时不支持负数"},
  {STREAM_WRITE_FAILED, "序列化写入失败"},
  {STREAM_READ_FAILED, "序列化读取失败"},
  {STREAM_MAX_VALUE, "序列化最大值"},
  {BITSETIO_PTR_NULL, "bitsetio new failed"},
  {BITSETIO_BITFILE_OPEN_FAILED, "创建bitfile失败"},
  {BITSETIO_BITFILE_LENGTH, "bitfile长度不够"},
  {BITSETIO_BITFILE_READ_FAILED, "bitfile读取失败"},
  {BITSETIO_BITFILE_FILEPATH_EMPTY, "bitfile文件名为空"},

  // 
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
  {CHUNKMANAGER_SIZE_MIN_LIMIT, "chunkmanager最少开放MB磁盘空间！"},
  {CHUNKMANAGER_NEED_ONUPDATE, "chunkmanager需求onUpdate回调类"},
  {CHUNKMANAGER_NEED_SPACEPATH, "chunkmanager未获取可用空间"},
  {CHUNKMANAGER_BLOCKCOUNT_NOT, "chunkmanager总块量不够"},
  {CHUNKMANAGER_CUTDOWN_BLOCKCOUNT, "chunkmanager暂时不支持缩减空间"},
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
