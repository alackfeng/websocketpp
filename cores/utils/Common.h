#ifndef SDK_CORES_UTILS_COMMON_H
#define SDK_CORES_UTILS_COMMON_H

#include <stdio.h>
#include <memory.h>
#include <mutex>
#include <unistd.h>
#include <cctype>

#ifndef CHUNKCC_DISORDER
#define CHUNKCC_DISORDER 1
#endif


#ifndef int64
#define int64 long long
#endif

#ifndef int32
#define int32 int
#endif

#ifndef int8
#define int8 unsigned char
#endif

#define KB 1024
#define MB 1024 * 1024
#define GB 1024 * 1024 * 1024
#define TB 1024 * 1024 * 1024 * 1024L
#define BLOCK_SIZE 64 * KB
#define BLOCK_BITS 16
#define BLOCK_MASK 0xFFFF
// chunkmanager use
#define DEFAULT_BLOCK_SIZE BLOCK_SIZE
#define DEFAULT_BIG_FILE_SIZE TB
#define DEFAULT_BIG_FILE_BLOCK_COUNT (DEFAULT_BIG_FILE_SIZE / DEFAULT_BLOCK_SIZE)
#define MIN_BIG_FILE_SIZE 1L * 16 * 1024 * 1024
#define MIN_BIG_FILE_BLOCK_COUNT (MIN_BIG_FILE_SIZE / DEFAULT_BLOCK_SIZE)


#ifndef POSIX_LIB
#define POSIX_LIB
#if defined(_WIN32) || defined(_WIN64)
#define setFilePos _fseeki64
#define getFilePos _ftelli64
#define truncFile _chsize_s
#define min __min
#else
#define setFilePos fseeko
#define getFilePos ftello
#define truncFile truncate
// #define min min
#endif
#endif


inline std::string& toUpper(std::string& dig) {
  for(int j=0; j<dig.size(); ++j) {
    dig[j] = std::toupper(dig[j]);
  }
  return dig;
}
inline std::string& toLower(std::string& dig) {
  for(int j=0; j<dig.size(); ++j) {
    dig[j] = std::tolower(dig[j]);
  }
  return dig;
}

inline unsigned long long strtollSecureKey(const char *key) {
  if(!key)
    return 0;
  return strtoull(key,NULL,16);
}


#define closeFile(file) if (file != NULL){fclose(file);file=NULL;}

inline int getBlockCount(int64 size) {
  return (int)((size + BLOCK_SIZE - 1) >> BLOCK_BITS);
}

inline int64 readFile(FILE* file, int64 fileOffset, void* buffer, size_t len) {
  setFilePos(file, fileOffset, SEEK_SET);
  size_t readSize = fread(buffer, 1, len, file);
  if (readSize < len)
    memset((char*)buffer + readSize, 0, len - readSize);
  return readSize;
}

inline int64 writeFile(FILE* file, int64 fileOffset, const void* buffer, size_t len) {
  setFilePos(file, fileOffset, SEEK_SET);
  return fwrite(buffer, 1, len, file);
}

inline int64 getFileLength(FILE* file) {
  if(!file)
    return -1;
  setFilePos(file, 0L, SEEK_END);
  return getFilePos(file);
}

inline int flushFile(FILE* file) {
  if(!file)
    return -1;
  return fflush(file);
}


#define localLock(lockObject) unique_lock<recursive_mutex> locker_##lockObject(lockObject)
#define lockThis() unique_lock<recursive_mutex> locker_this(*this)
#define synchronized(lockObject)  for(localLock(lockObject); locker_##lockObject.owns_lock(); locker_##lockObject.unlock())
#define syncThis()  for(lockThis(); locker_this.owns_lock(); locker_this.unlock())

#define localLockPtr(lockObject) unique_lock<recursive_mutex> locker_##lockObject(*lockObject)
#define syncPtr(lockObject)  for(localLockPtr(lockObject); locker_##lockObject.owns_lock(); locker_##lockObject.unlock())

#endif