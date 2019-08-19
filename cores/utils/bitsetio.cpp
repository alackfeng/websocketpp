
#include "bitsetio.hpp"

namespace sdk {
namespace cores {
namespace utils {

bool bitsetio::init_bitfile(const string& filepath) {
  this->filepath = filepath;
  int bitfilesize = BITSIZE(size_);

  bffile.open(filepath, ios::app|ios::out|ios::in);
  if (!bffile.is_open()) {
    throw 111;
  }
  // 写入初始数据，并大小
  char buffer[4096] = {0};
  for(int offset = 0; offset < bitfilesize; offset += 4096) {
    bffile.seekg(offset, ios::beg);
    bffile.write(buffer, std::min(4096, bitfilesize-offset));
  }
  // 文件大小
  streampos size = bffile.tellg();
  if (size != bitfilesize) {
    throw 112;
  }
  return true;
}

bool bitsetio::load_bitfile(const string& filepath) {
  
  bffile.open(filepath, ios::app|ios::out|ios::in);
  if (!bffile.is_open()) { // "ChunkManager元数据损坏," + bitFilePath + "文件不存在！
    throw 111;
  }
  // 检验文件大小
  int bitfilesize = BITSIZE(size_);
  streampos size = bffile.tellg();
  if (size != bitfilesize) {
    throw 112;
  }

  char* buffer = new char[bitfilesize];
  bffile.read(buffer, bitfilesize);
  for(int index = 0; index < bitfilesize; ++index)
    this->set(index);
  if (buffer) delete []buffer;
  // 读取缓存内容，设置bitset缓存



  return true;
}

bool bitsetio::set_bitfile(int startindex, int endindex, bool value) {
  for(int i=startindex; i<endindex; ++i) {
    set(i); // true or false
  }
  // write to file
  return true;
}


} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {
