#pragma once
#include "Common.h"
#include "Exception.h"
using namespace std;
using namespace sdk::cores::utils;

namespace sdk {
namespace cores {
namespace utils {

class SyncFile : public recursive_mutex {
protected:
  char filename[256] = {0};
  FILE* file;
  void open(const char* _filename, bool _onlyRead);
public:
  explicit SyncFile(const char* _filename, bool _onlyRead);
  explicit SyncFile();
  ~SyncFile();
  size_t setLength(int64 fileSize);
  inline int64 getLength() {
    return ::getFileLength(file);
  }
  inline void seek(int64 offset) {
    if (setFilePos(file, offset, SEEK_SET) == -1) throw CodeException::SYNCFILE_FILE_SETLENGHT_FAILED;
  }
  inline int64 pos() { return getFilePos(file); }
  int64 read(int64 fileOffset, void* buffer, int len);
  int64 write(int64 fileOffset, const void* buffer, int len);
  // 原生读写
  int64 readL(int64 fileOffset, void* buffer, int len);
  int64 writeL(int64 fileOffset, void* buffer, int len);
  size_t fileSize;
  bool isReadOnly = false;
  void close();
  void deleteFile();
  void init(const char* _filename, bool _onlyRead);
  int truncFile(int64 len);
  bool isOpen();
};

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {
  