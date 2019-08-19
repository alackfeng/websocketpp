
#include "chunk.hpp"
#include <cores/chunk/chunkinterface.hpp>


namespace sdk {
namespace cores {
namespace chunk {

string chunk::CHUNK_VERSION = "A.B";

chunk::chunk(chunkmanager* manager) {
  std::cout << "chunk:" << __FUNCTION__ << std::endl;
  this->manager = manager;
}
chunk::chunk(chunkmanager* manager, instreamhelp& is) {
  this->manager = manager;
  read(is);
}
chunk::~chunk() {
  std::cout << "chunk:" << __FUNCTION__ << std::endl;
}

chunk* chunk::init(string chunkid, int chunksize) {
  this->chunkid = chunkid;
  this->chunksize = chunksize;
  return this;
}
chunk* chunk::loadfrom(char* chunkinfo) {
  readFromBytes(chunkinfo);
  return this;
}

bool operator==(const chunk& ck, const chunk& ock) {
  std::cout << "chunk:" << __FUNCTION__ << " ck1 " << ck.chunkid << ", size " << ck.chunksize << std::endl;
  return ck.chunkid == ock.chunkid;
}
ostream &operator<<(ostream &os, const chunk &ck) {
  std::cout << "chunk:" << __FUNCTION__ << std::endl;
  int v = chunk::CHUNK_VERSION.size();
  int versize = ((v >> 24) & 0xFF) | ((v >> 16) & 0xFF) | ((v >> 8) & 0xFF) | ((v >> 0) & 0xFF);
  os << versize;
  os << chunk::CHUNK_VERSION;
  os << ck.chunkid;
  os << ck.chunksize;
  os << ck.datasize;
  // os << ck.strips.size();
  os << ck.deadtime;
  return os;
}
istream &operator>>(istream &is, chunk& ck) {
  string ver; // chunk version
  is >> ver;
  std::cout << "chunk:" << __FUNCTION__ << " version " << ver << std::endl;
  is >> ck.chunkid;
  is >> ck.chunksize;
  is >> ck.datasize;
  is >> ck.deadtime;
  return is;
}
void chunk::write(outstreamhelp &os) {
  os.writeUTF(CHUNK_VERSION);
  os.writeUTF(chunkid);
  os.writeInt(chunksize);
  os.writeInt(datasize);
  os.writeInt(strips.size());
  for (list<stripinfo>::iterator i = strips.begin(); i != strips.end(); ++i)
  {
    os.writeInt(i->startindex);
    os.writeInt(i->blockcount);
  }
  os.writeLong(deadtime);

}
void chunk::read(instreamhelp &is) {
  string ver = is.readUTF();
  if(ver != CHUNK_VERSION) {
    throw 176; // 无法识别Chunk版本信息
  }
  chunkid = is.readUTF();
  chunksize = is.readInt();
  datasize = is.readInt();
  int stripcount = is.readInt();
  while(stripcount > 0) {
    int startindex = is.readInt();
    int blockcount = is.readInt();
    strips.emplace_back(stripinfo(startindex, blockcount));
    --stripcount;
  }
  // is >> *this;
  // int versize;
  // is >> versize;
  // string ver; // chunk version
  // is >> ver;
  // is.read(reinterpret_cast<char*>(this), sizeof(this));

  std::cout << "chunk:" << __FUNCTION__ << " version " << this->chunksize << std::endl;
}

void chunk::extract_blocktargets() {
  int blockCount = this->get_blockcount();
  blocktargets = new bigfile::blocktarget[blockCount];
  int index = 0;
  bigfile_ptr lastBigFile = manager->get_bigfile(0);
  for (stripinfo stripInfo : strips) {
    int endIndex = stripInfo.startindex + stripInfo.blockcount;
    for (int blockIndex = stripInfo.startindex; blockIndex < endIndex; ++blockIndex) {
      bigfile::blocktarget* target = lastBigFile->create_blocktarget(blockIndex);
      if (target == NULL) {
        target = manager->create_blocktarget(blockIndex);
        if (target == NULL)
          throw 157; // new BitDiskException("块索引异常！", _C_RD.CMCKBlockIndexException);
        // lastBigFile = target->get_bigfile();
      }


      if (index >= blockCount) // strips数据异常处理
        throw 195; //  new BitDiskException("块索引异常！", _C_RD.CMCKBlockIndexException);
      blocktargets[index++] = *target;
    }
  }
}

int chunk::writedata(int offsetInChunk, char* data, int len) {
  if (offsetInChunk > chunksize)
    throw 999; // new BitDiskException("写入数据时超界！", _C_RD.CMCKOutOfBounds);
  int newChunkSize = offsetInChunk + len;
  if (newChunkSize > chunksize) { // 扩展数据长度
    int blockCount = manager->calc_blockcount(chunksize);
    int newBlockCount = manager->calc_blockcount(newChunkSize);
    if (newBlockCount > blockCount) {
      list<stripinfo> strips = manager->alloc_blockstrips(newBlockCount - blockCount);
      addstrips(strips);
      chunksize = newChunkSize;
      extract_blocktargets();
    }
  }
  if (blocktargets == NULL)
    extract_blocktargets();
  int blockSize = manager->get_blocksize();
  int remainLen = len;
  while (remainLen > 0) {
    int targetIndex = offsetInChunk / blockSize;
    bigfile::blocktarget target = blocktargets[targetIndex];
    int offsetInBlock = offsetInChunk - targetIndex * blockSize;
    int writeSize = target.writedata(offsetInBlock, data, len - remainLen, remainLen);
    remainLen -= writeSize;
    offsetInChunk += writeSize;
    if (offsetInChunk > datasize)
      datasize = offsetInChunk;
    if (datasize > chunksize)
      chunksize = datasize;
  }
  if (manager->on_update() != NULL)
    manager->on_update()->onUpdateChunk(chunkid, get_chunkinfo());
  return len - remainLen;
}
int chunk::readdata(int offsetInChunk, char* data, int len) {
  if (offsetInChunk >= chunksize)
    return 0;
  if (blocktargets == NULL)
    extract_blocktargets();
  int blockSize = manager->get_blocksize();
  int remainLen = len;
  while (remainLen > 0) {
    int targetIndex = offsetInChunk / blockSize;
    bigfile::blocktarget target = blocktargets[targetIndex];
    int offsetInBlock = offsetInChunk - targetIndex * blockSize;
    int readSize = target.readdata(offsetInBlock, data, len - remainLen, remainLen);
    remainLen -= readSize;
    offsetInChunk += readSize;
  }
  return len - remainLen;
}

int chunk::get_blockcount() { return manager->calc_blockcount(chunksize); }


} /// namespace chunk {
} /// namespace cores {
} /// namespace sdk {
