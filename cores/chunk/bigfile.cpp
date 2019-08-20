
#include "bigfile.hpp"
// #include <cores/chunk/chunk.hpp>
#include <cores/chunk/chunkinterface.hpp>

using namespace sdk::cores::utils;

namespace sdk {
namespace cores {
namespace chunk {

string bigfile::BIG_FILE_VERSION = "1.0";

bigfile::bigfile() {
  LOG_F(ERROR, "bigfile::bigfile call");
}
bigfile::bigfile(chunkmanager* manager, string filepath, int startindex, int blockcount)
:manager(manager), startindex(startindex), blockcount(blockcount), filepath(filepath), SyncFile() {

  version = bigfile::BIG_FILE_VERSION;
  LOG_F(INFO, "bigfile::bigfile call. %p, version %s, filepath %s, startindex %d, blockcount %d.", 
    manager, version.c_str(), filepath.c_str(), startindex, blockcount);  

  this->open(filepath.c_str(), false); // 创建读写文件
  int64 oldlen = this->setLength(this->get_size()); // 初始设置文件大小
  if(oldlen != 0 && oldlen != this->getLength()) { // 文件大小改变不允许
    LOG_F(ERROR, "bigfile::bigfile - filepath %s size fixed", filepath.c_str());
    throw sdkexception(BIGFILE_SIZE_FIXED);
  }
  LOG_F(INFO, "bigfile::bigfile - create bigfile filepath %s, setLength %d", filepath.c_str(), this->get_size());
}

bigfile::bigfile(chunkmanager* manager, instreamhelp& is)
:manager(manager) {
  version = is.readUTF();
  if(BIG_FILE_VERSION != version) {
    LOG_F(ERROR, "bigfile::bigfile - filepath %s not fix version", filepath.c_str());
    throw sdkexception(BIGFILE_VERSION_NOT_FIT); // 无法识别存储体的版本信息
  }
  string managerid = is.readUTF();
  if (manager->get_id() != managerid) {
    LOG_F(ERROR, "bigfile::bigfile - filepath %s not fix ChunkManager ID", filepath.c_str());
    throw sdkexception(CHUNKMANAGER_ID_NOT_FIX); // ChunkManager 不匹配
  }
  filepath = is.readUTF();
  startindex = is.readInt();
  blockcount = is.readInt();
  if (startindex % 8 != 0 || blockcount % 8 != 0) { // bitset索引倍数
    // "ChunkManager元数据损坏！startIndex:" + startIndex + " ,blockCount" + blockCount
    LOG_F(ERROR, "bigfile::bigfile - filepath %s not fix chunkmanager meta breakdown ", filepath.c_str());
    throw sdkexception(CHUNKMANAGER_META_BREAKDOWN);
  }

  this->open(filepath.c_str(), false);
  if(!this->isOpen() || this->getLength() != get_size()) { // 检测文件存在与否
    // 文件丢失，标记一下
    LOG_F(ERROR, "bigfile::bigfile - filepath %s not exist or change size ", filepath.c_str());
    throw sdkexception(BIGFILE_NOT_EXIST);
  }
}

bigfile::~bigfile() {
  LOG_F(INFO, "bigfile::~bigfile call. filepath %s", filepath.c_str());
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
    // 不属于本bigfile范围内
    LOG_F(ERROR, "blockindex %d not between %d and %d", blockindex, startindex, startindex + blockcount);
    return NULL;
  }

  // 读取是否在索引文件
  LOG_F(WARNING, "create_blocktarget - filepath %s blockindex %d, 读取是否在索引文件", filepath.c_str(), blockindex);

  LOG_F(INFO, "create_blocktarget - filepath %s blockindex %d, at %d-%d", 
    filepath.c_str(), blockindex, startindex, blockcount);
  return new blocktarget(self(), blockindex);
}


/////////////////bigfile::blocktarget+++++++++++++++++++++
bigfile::blocktarget::blocktarget(): block_startoffset_infile(-1) {
  LOG_F(ERROR, "blocktarget::blocktarget call. block_startoffset_infile %d.", block_startoffset_infile);
}
bigfile::blocktarget::blocktarget(bigfile_ptr bf, int blockindex): bfile(bf) {
  int startindex = bfile->startindex;
  block_startoffset_infile = (int64)(blockindex - startindex) * bfile->get_blocksize();
  
  LOG_F(INFO, "blocktarget::blocktarget call. block_startoffset_infile %d.", block_startoffset_infile);
  if(block_startoffset_infile < 0) {
    LOG_F(ERROR, "blocktarget::blocktarget - startoffset infile <0");
    throw sdkexception(BLOCKTARGET_NO_IN_BIGFILE);
  }
}
bigfile::blocktarget::~blocktarget() {
  LOG_F(INFO, "blocktarget::~blocktarget call. block_startoffset_infile %d.", block_startoffset_infile);
}

int bigfile::blocktarget::writedata(int offsetinblock, const char* data, int offset, int len) {
  if (!bfile || bfile.use_count() == 0) {
    LOG_F(ERROR, "blocktarget::writedata - get bigfile ptr released");
    throw sdkexception(BIGFILE_PTR_RELEASED);
  }

  LOG_F(INFO, "blocktarget::writedata - write offsetinblock %d, offset %d,  len %d, at bigfile %d", 
    offsetinblock, offset, len, block_startoffset_infile);

  int blocksize = bfile->get_blocksize();
  if(offsetinblock >= blocksize) {
    LOG_F(ERROR, "blocktarget::writedata - write offset max limit");
    throw sdkexception(BLOCKTARGET_MAX_LIMIT);
  }
  // 超过了大小
  if(offsetinblock + len > blocksize) {
    LOG_F(ERROR, "blocktarget::writedata - write offset max limit");
    throw sdkexception(BLOCKTARGET_MAX_LIMIT);
  }

  LOG_F(WARNING, "blocktarget::writedata write failed, update chunkmanager bitset");
  return bfile->write(block_startoffset_infile + offsetinblock, data + offset, len);
}
int bigfile::blocktarget::readdata(int offsetinblock, char* data, int offset, int len) {
  if (!bfile || bfile.use_count() == 0) {
    LOG_F(ERROR, "blocktarget::readdata - get bigfile ptr released");
    throw sdkexception(BIGFILE_PTR_RELEASED);
  }

  LOG_F(INFO, "blocktarget::readdata - read offsetinblock %d, offset %d,  len %d, at bigfile %d", 
    offsetinblock, offset, len, block_startoffset_infile);

  int blocksize = bfile->get_blocksize();
  if(offsetinblock >= blocksize) {
    LOG_F(ERROR, "blocktarget::readdata - read offset max limit");
    throw sdkexception(BLOCKTARGET_MAX_LIMIT);
  }
  // 超过了大小
  if(offsetinblock + len > blocksize) {
    LOG_F(ERROR, "blocktarget::readdata - read offset max limit");
    throw sdkexception(BLOCKTARGET_MAX_LIMIT);
  }

  LOG_F(WARNING, "blocktarget::readdata read failed, update chunkmanager bitset");
  return bfile->read(block_startoffset_infile + offsetinblock, data + offset, len);
}

int bigfile::get_blocksize() { return manager->get_blocksize(); }
int64 bigfile::get_size() { return (int64)get_blocksize() * blockcount; }
bool bigfile::valid() { return this->isOpen(); }
} /// namespace chunk {
} /// namespace cores {
} /// namespace sdk {
