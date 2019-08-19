
#ifndef SDK_CORES_FILEIO_H
#define SDK_CORES_FILEIO_H

#include "Common.h"
#include <iostream>
#include <fstream>


namespace sdk {
namespace cores {
namespace utils {

using namespace std;
class fileio
{
public:
  explicit fileio(const char* filename, bool readonly);
  explicit fileio() {};
  ~fileio();
  void open(const char* filename, bool readonly);
  int64 setLength(int64 fileSize);
  inline int64 getLength();

  int64 read(int64 fileOffset, void* buffer, int len);
  int64 write(int64 fileOffset, const void* buffer, int len);
  void close();
  void deleteFile();
  int truncFile(int64 len);


  inline bool isOpen() { return fs.is_open(); }
  inline void seek(int64 offset) {  fs.seekg(offset, ios::cur); }
  inline int64 pos() { return fs.tellg(); }

private:
  string filename;
  fstream fs;
  int64 filesize;
  bool readonly = false;
  
};

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {




#endif