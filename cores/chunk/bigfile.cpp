
#include "bigfile.hpp"
// #include <cores/chunk/chunk.hpp>
#include <cores/chunk/chunkinterface.hpp>

using namespace sdk::cores::utils;

namespace sdk {
namespace cores {
namespace chunk {

string bigfile::BIG_FILE_VERSION = "1.0";

bigfile::bigfile(chunkmanager* manager, string filepath, int startindex, int blockcount)
:manager(manager), startindex(startindex), blockcount(blockcount), filepath(filepath) {

  version = bigfile::BIG_FILE_VERSION;

  this->open(filepath.c_str(), false);
  this->setLength(this->get_size());
}

bigfile::bigfile(chunkmanager* manager, instreamhelp& is)
:manager(manager) {
  version = is.readUTF();
  if(BIG_FILE_VERSION != version) {
    throw 198; // 无法识别存储体的版本信息
  }
  string managerid = is.readUTF();
  if (manager->get_id() != managerid) {
    throw 178; // Chunk Manager 不匹配
  }
  filepath = is.readUTF();
  startindex = is.readInt();
  blockcount = is.readInt();
  if (startindex % 8 != 0 || blockcount % 8 != 0) {
    throw 179; // "ChunkManager元数据损坏！startIndex:" + startIndex + " ,blockCount" + blockCount
  }

  this->open(filepath.c_str(), false);
  if(!this->isOpen() || this->getLength() != get_size()) { // 检测文件存在与否
    ; // 文件丢失，标记一下
  } else {
    this->open(filepath.c_str(), false);
  }
}

bigfile::~bigfile() {

}

void bigfile::writeinfo(outstreamhelp& os) {
  os.writeUTF(version);
  os.writeUTF(manager->get_id());
  os.writeUTF(filepath);
  os.writeInt(startindex);
  os.writeInt(blockcount);

}

bigfile::blocktarget* bigfile::create_blocktarget(int blockindex) {
  if (blockindex < startindex || blockindex >= startindex + blockcount) {
    // 不属于我
    return NULL;
  }

  return new blocktarget(bigfile_ptr(this), blockindex);
}


int bigfile::blocktarget::writedata(int offsetinblock, const char* data, int offset, int len) {
  if (!bfile) {
    throw CodeException::DYNMAIC_PTRNULL;
  }
  int blocksize = bfile->get_blocksize();
  if(offsetinblock > blocksize) {
    return 0;
  }
  if(len + offsetinblock > blocksize) {
    throw ChunkError::OFFSETINBLOCK_MAX;
  }

  return bfile->write(block_startoffset_infile + offsetinblock, data + offset, len);
}
int bigfile::blocktarget::readdata(int offsetinblock, char* data, int offset, int len) {
  if (!bfile) {
    throw CodeException::DYNMAIC_PTRNULL;
  }
  int blocksize = bfile->get_blocksize();
  if(offsetinblock > blocksize) {
    return 0;
  }
  if(len + offsetinblock > blocksize) {
    throw ChunkError::OFFSETINBLOCK_MAX;
  }

  return bfile->read(block_startoffset_infile + offsetinblock, data + offset, len);
}

int bigfile::get_blocksize() { return manager->get_blocksize(); }
int64 bigfile::get_size() { return (int64)get_blocksize() * blockcount; }
} /// namespace chunk {
} /// namespace cores {
} /// namespace sdk {
