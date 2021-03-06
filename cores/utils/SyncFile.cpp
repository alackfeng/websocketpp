#include "SyncFile.hpp"
#include "bdlog.hpp"

using namespace sdk::cores::utils;
namespace sdk {
namespace cores {
namespace utils {


SyncFile::SyncFile(): file(nullptr) {}
void SyncFile::init(const char* _filename, bool _onlyRead) {
  
  // 文件名拷贝
  this->filename = _filename;
  this->isReadOnly = _onlyRead;
  this->open(filename.c_str(), isReadOnly);
}

SyncFile::SyncFile(const char *_filename, bool _onlyRead) : file(nullptr), isReadOnly(_onlyRead) {
  // 文件名拷贝
  this->filename = _filename;
  open(filename.c_str(), isReadOnly);
}

void SyncFile::open(const char* _filename, bool _isReadOnly){
  file = fopen(_filename, _isReadOnly ? "r" : "r+");
  if (!file){
    bdlog_info("SyncFile::open %s\n", _filename);
    if(_isReadOnly) {
      bdlog_error("SyncFile::open readonly ERROR SYNCFILE_FILE_OPEN_FAILED %s\n", _filename);
      throw CodeException::SYNCFILE_FILE_OPEN_FAILED;
    }
    file = fopen(_filename, "w+");
    if(!file) {
      bdlog_error("SyncFile::open w+ ERROR SYNCFILE_FILE_OPEN_FAILED %s\n", _filename);
      throw CodeException::SYNCFILE_FILE_OPEN_FAILED;
    }
  }
    
  setFilePos(file, 0, SEEK_END);
  fileSize = getFilePos(file);
  setFilePos(file, 0, SEEK_SET);

  // 文件名拷贝
  this->filename = _filename;
  this->isReadOnly = _isReadOnly;
}

SyncFile::~SyncFile() {
  closeFile(file);
}

size_t SyncFile::setLength(int64 _fileSize) {
  syncThis() {
    if (!file) {
      bdlog_error("SyncFile::setLength - ERROR SYNCFILE_FILE_SETLENGHT_FAILED %s\n", filename.c_str());
      throw CodeException::SYNCFILE_FILE_SETLENGHT_FAILED;
    }
    if (fileSize == _fileSize)  return fileSize;
    if (isReadOnly) {
      bdlog_error("SyncFile::setLength - readonly ERROR SYNCFILE_FILE_SETLENGHT_FAILED %s\n", filename.c_str());
      throw CodeException::SYNCFILE_FILE_SETLENGHT_FAILED;
    }

    fclose(file);
    bdlog_info("SyncFile::setLength - filename %s\n", filename.c_str());
    file = fopen(filename.c_str(), "w+");
    if(!file) {
      bdlog_error("SyncFile::setLength - w+ ERROR SYNCFILE_FILE_SETLENGHT_FAILED %s\n", filename.c_str());
      throw CodeException::SYNCFILE_FILE_SETLENGHT_FAILED;
    }
    seek(_fileSize - 1);
    if (fputc(0, file) == EOF) {
      bdlog_error("SyncFile::setLength - fputc ERROR SYNCFILE_FILE_SETLENGHT_FAILED %s\n", filename.c_str());
      throw CodeException::SYNCFILE_FILE_SETLENGHT_FAILED;
    }
    fclose(file);

    int oldSize = fileSize;
    open(filename.c_str(), isReadOnly);
    return oldSize;
  }
  return 0;
}

int64 SyncFile::read(int64 fileOffset, void *buffer, int len) {
  syncThis() {
    int64 readSize = readFile(file, fileOffset, buffer, len);
    if(readSize != len) {
      bdlog_error("SyncFile::read - ERROR read file %s<%p>. %d - %d:%d.\n", filename.c_str(), file, fileOffset, readSize, len);
      // throw CodeException::FILE_READ_OP_ERROR;
    }
    return readSize;
  }
  return 0;
}

int64 SyncFile::write(int64 fileOffset, const void *buffer, int len) {
  syncThis() {
    int64 writeSize = writeFile(file, fileOffset, buffer, len);
    if(writeSize != len) {
      bdlog_error("SyncFile::write - ERROR write file %s. %d - %d:%d.\n", filename.c_str(), fileOffset, writeSize, len);
      // throw CodeException::FILE_WRITE_OP_ERROR;
    }
    return writeSize;
  }
  return 0;
}

// 原生读写
int64 SyncFile::readL(int64 fileOffset, void* buffer, int len) {
  syncThis() {
    setFilePos(file, fileOffset, SEEK_SET);
    size_t readSize = fread(buffer, 1, len, file);
    return readSize;
  }
  return 0;
}
int64 SyncFile::writeL(int64 fileOffset, void* buffer, int len) {
  syncThis() {
    setFilePos(file, fileOffset, SEEK_SET);
    size_t writeSize = fwrite(buffer, 1, len, file);
    return writeSize;
  }
  return 0;
}


void SyncFile::close() {
syncThis() {
  bdlog_info("SyncFile::close %s\n", this->filename.c_str());
  closeFile(file);
  file = nullptr;
}
}

void SyncFile::deleteFile() {
syncThis() {
  if(file) closeFile(file);
   printf("SyncFile::deleteFile %s\n", this->filename.c_str());
  remove(filename.c_str());
}
}

int SyncFile::truncFile(int64 len) {
#ifdef WIN32
  if (!isOpen()) open();
  if (file) {
    //printf("SyncFile::deleteFile %p, len %d\n", this->file, len);
    return ::truncFile(_fileno(file), len);
  }
#else
  if(filename != "") {
    return ::truncFile(filename.c_str(), len);
  }
#endif
  return 0;
}

bool SyncFile::isOpen() {
  return file != nullptr;
}
int SyncFile::flushFile() {
  return ::flushFile(file);
}

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {