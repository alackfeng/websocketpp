
#include "chunk_manager.hpp"
#include "cores/utils/fileio.hpp"
#include "cores/chunk/chunk.hpp"

// using namespace sdk;
using namespace sdk::plugins;
using namespace sdk::cores::utils;


string chunk_manager::CHUNK_MANAGER_VERSION_1_0 = "1.0";
string chunk_manager::CHUNK_MANAGER_VERSION_1_1 = "1.1";


chunk_manager::chunk_manager(string id, string cmpath, int sizemb, onupdate* update)
:update(update) {
  std::cout << "chunk_manager::" << __FUNCTION__ << ", id " << id 
    << ", path: " << cmpath << ", size " << sizemb << std::endl;

  this->version = chunk_manager::CHUNK_MANAGER_VERSION_1_1;
  this->id = id;
  this->blocksize = DEFAULT_BLOCK_SIZE;
  int64 size = sizemb * 1024L * 1024L;
  this->totalblock = int(size / blocksize);
  if (totalblock < MIN_BIG_FILE_BLOCK_COUNT) { // "请至少开放" + (totalBlock * (blockSize / 1024) / 1024) + "MB磁盘空间！"
    throw 10;
  }
  this->usedblock = 0;

  // 创建索引文件缓存，大小为一个bigfile的block数
  std::unique_ptr<bitsetio> bsetindex(new bitsetio(totalblock));
  bsetio = std::move(bsetindex);

  // 获取索引文件可用磁盘空间路径
  int bitsize = BITSIZE(totalblock); // bsetio->size();
  string filepathbit = update->getSpacePath(bitsize);
  if (filepathbit == "") { // 无法获取有效的磁盘空间，可使用空间如何判断？length
    throw 100;
  }
  filepathbit += this->id + ".bs"; // 索引文件,标记block是否被使用了
  bsetio->init_bitfile(filepathbit);

  this->totalblock = 0;
  this->filepath = cmpath + "chunks.cm";
  resize(sizemb);
  writeto_file(this->filepath);

  std::cout << "update " << this->update.get() << std::endl;
}
chunk_manager::chunk_manager(string id, string filepath, onupdate* update) {
  this->update = std::unique_ptr<onupdate>(update);
  this->filepath = filepath + "chunks.cm";
  readfrom_file(this->filepath);
  if (id != "" && this->id != id) {
    throw 115; // 数据文件损坏
  }

}

onupdate* chunk_manager::on_update() {
  if (update) return update.get();
  return NULL;
}

bool chunk_manager::is_intact() {
  list<std::unique_ptr<bigfile>>::iterator itr;
  for(itr = bigfiles.begin(); itr != bigfiles.end(); ++itr) {
    if (!itr->get()->data()) {
      return false;
    }
  }
  return true;
}

void chunk_manager::flush_bits(list<chunk::stripinfo>& strips, bool value) {
  list<chunk::stripinfo>::iterator itr = strips.begin();
  for(; itr != strips.end(); ++itr) {
    chunk::stripinfo sinfo = *itr;
    if(!bsetio->set_bitfile(sinfo.startindex, sinfo.startindex + sinfo.blockcount, value)) {
      throw 980; // fail
    }
    if(value) usedblock += sinfo.blockcount;
    else usedblock -= sinfo.blockcount;
  }

  update_spaceinfo();
}

chunk* chunk_manager::loadchunk(string chunkid, string chunkinfo) {
  chunk* ck = new chunk(reinterpret_cast<sdk::cores::chunk::chunkmanager*>(this));
  ck->loadfrom((char*)(chunkinfo.c_str()));

  list<chunk::stripinfo> strips = ck->getstrips();

  list<std::unique_ptr<bigfile>>::iterator itr;
  for(itr = bigfiles.begin(); itr != bigfiles.end(); ++itr) {
    bigfile* bf = itr->get();
    if (!bf->data()) {
      continue;
    }

    int endindex = bf->start_index() + bf->get_blockcount();
    list<chunk::stripinfo>::iterator it = strips.begin();
    for(; it != strips.end(); ++it) {
      chunk::stripinfo sinfo = *it;
      if(sinfo.startindex >= bf->start_index() && sinfo.startindex < endindex) {
        throw 985; // Chunk数据已经损坏
      }
      int stripend = sinfo.startindex + sinfo.blockcount;
      if(stripend >= bf->start_index() && stripend < endindex) {
        throw 984; // Chunk数据已经损坏
      }
    }

  }
  return ck;
}

int chunk_manager::find_onestrip(int blockcount) {
  int nextClear = 0, nextSet = 0, searchStart = 0;
  do {
    // nextClear = bitSet.nextClearBit(searchStart);// 下一个可用块的起始位置
    if (nextClear < 0)
      return -1;
    // nextSet = bitSet.nextSetBit(nextClear);// 该可用块的结束位置
    if (nextSet < 0) {// 已搜索到整个ChunkManager的末尾
      if (nextClear + blockcount < totalblock)// 末尾可用块空间足够
        return nextClear;
      return -1;
    }
    searchStart = nextSet;
  } while (nextSet - nextClear < blockcount);
  return nextClear;
}

list<chunk::stripinfo>* chunk_manager::alloc_blockstrips(int blockCount) {
  list<chunk::stripinfo>* strips = new list<chunk::stripinfo>;
  int startindex = find_onestrip(blockCount);
  if (startindex >= 0) {
    strips->emplace_back(chunk::stripinfo(startindex, blockCount));
  } else {
    // 没有找到连续块，尝试使用碎片
    int allocCount = 0;
    while (allocCount < blockCount) {
      int nextClear = 0; //bitSet.nextClearBit(0);
      if (nextClear < 0)
        throw 983; // new BitDiskException("剩余空间不足！");
      int nextSet = 0; //bitSet.nextSetBit(nextClear);
      int currentCount = nextSet - nextClear;
      if (allocCount + currentCount >= blockCount)
        currentCount = blockCount - allocCount;
      strips->emplace_back(chunk::stripinfo(nextClear, currentCount));
      allocCount += currentCount;
    }
  }

  flush_bits(*strips, true);
  return strips;
}

void chunk_manager::check_locks() {
  // Date currentTime = new Date();
  // synchronized (lockedChunks) 
  {
    list<std::unique_ptr<chunk>>::iterator itr = lockedchunks.begin();
    for(; itr != lockedchunks.end(); ++itr) {
      // if (chunk.isAlive(currentTime))
      //   continue;
      
      // itr.remove();
      flush_bits(itr->get()->getstrips(), false);
    } 
  } 
}

/**
 * 为文件片分配空间并锁定
 **/
chunk* chunk_manager::alloc_chunk(string chunkid, int chunksize, int lifelen) {
  // synchroniszed
  check_locks();
  int blockcount = calc_blockcount(chunksize);
  list<chunk::stripinfo>* strips = alloc_blockstrips(blockcount);
  chunk* ck = new chunk(reinterpret_cast<sdk::cores::chunk::chunkmanager*>(this));
  ck->init(chunkid, chunksize)->addstrips(*strips)->set_lifelen(lifelen);
  return ck;
}

/** 清空锁定Chunk */
void chunk_manager::clear_lockedchunk() {
  // synchroniszed
  // synchronized (lockedChunks) 
  {
    list<std::unique_ptr<chunk>>::iterator itr = lockedchunks.begin();
    for(; itr != lockedchunks.end(); ++itr) {
      flush_bits(itr->get()->getstrips(), false);
      // lockedchunks.remove(chunk);
    } 
    // lockedChunks.clear();
  } 
}

chunk* chunk_manager::find(string chunkid) {
  // synchronized (lockedChunks) 
  {
    list<std::unique_ptr<chunk>>::iterator itr = lockedchunks.begin();
    for(; itr != lockedchunks.end(); ++itr) {
      if(itr->get()->get_chunkid() == chunkid) {
        return itr->get();
      }
    } 
    return NULL;
  } 
}

/** 正常解锁，不会释放Chunk占用的空间 */
chunk* chunk_manager::unlock_chunk(string chunkid) {
  // synchronized (lockedChunks) 
  {
    chunk* pck = find(chunkid);
    if(!pck ) {
      throw 954; // null chunkid 并未加锁
    }
    // lockedchunks.remove(*pck);
    if(update)
      update->onUpdateChunk(chunkid, pck->get_chunkinfo());
    return pck;
  } 
}

/** 解锁并释放Chunk占用的空间，被锁定的chunk只存在于内存中 */
void chunk_manager::release_chunk(string chunkid) {
  // synchronized (lockedChunks) 
  {
    chunk* pck = find(chunkid);
    if(!pck ) {
      throw 954; // null chunkid 并未加锁
    }
    // lockedchunks.remove(*pck);
    flush_bits(pck->getstrips(), false);
  }
}

/** 解锁并释放Chunk占用的空间，被锁定的chunk只存在于磁盘中 */
void chunk_manager::release_chunk(chunk* chunk) {

}

void chunk_manager::write(outstreamhelp &os) {
  os.writeUTF(version);
  os.writeUTF(id);
  os.writeInt(blocksize);
  os.writeInt(totalblock);
  os.writeInt(usedblock);
  os.writeInt(blockcheckindex);
  os.writeUTF(bitfilepath);
  os.writeInt(bigfiles.size());
  for(list<std::unique_ptr<bigfile>>::iterator itr = bigfiles.begin(); itr != bigfiles.end(); ++itr) {
    itr->get()->writeinfo(os);
  }
  os.writeInt(lockedchunks.size());
  for(list<std::unique_ptr<chunk>>::iterator itr = lockedchunks.begin(); itr != lockedchunks.end(); ++itr) {
    itr->get()->writeinfo(os);
  } 

}
void chunk_manager::read(instreamhelp &is) {
  version = is.readUTF();
  blockcheckindex = (version == chunk_manager::CHUNK_MANAGER_VERSION_1_0) ? 0 : -1;
  id = is.readUTF();
  blocksize = is.readInt();
  totalblock = is.readInt();
  usedblock = is.readInt();
  if(blockcheckindex < 0) blockcheckindex = is.readInt();
  
  bitfilepath = is.readUTF();
  bsetio->load_bitfile(bitfilepath);

  int bigfilecount = is.readInt();
  int calcucount = 0;
  while(bigfilecount > 0) {
    bigfile* bigFile = new bigfile(reinterpret_cast<sdk::cores::chunk::chunkmanager*>(this), is);
    bigfiles.emplace_back(bigFile);
    calcucount += bigFile->get_blockcount();
    --bigfilecount;
  }
  if(calcucount != totalblock) {
    throw 987; // Block Count Error
  }

}

void chunk_manager::resize(int sizemb) {
  long newsize = sizemb * 1024L * 1024L;
  int newTotalBlock = (int) (newsize / blocksize);
  if (newTotalBlock < totalblock)
    throw 123; // new BitDiskException("抱歉，暂时不支持缩减空间！");
  int allocBlocks = totalblock;
  string lastBigFilePath = "";
  while (allocBlocks < newTotalBlock) {
    int blockCount = newTotalBlock - allocBlocks;
    if (blockCount > DEFAULT_BIG_FILE_BLOCK_COUNT)
    blockCount = DEFAULT_BIG_FILE_BLOCK_COUNT;
    long bigFileSize = (long) blockCount * blocksize;
    if (lastBigFilePath == "")
    lastBigFilePath = update->getSpacePath(bigFileSize);
    long freeSpace = 0; // not this, new File(lastBigFilePath).getFreeSpace();
    if (freeSpace < bigFileSize)
      blockCount = (int) (freeSpace / blocksize);
    int bfIndex = bigfiles.size();
    string bigFilePath;
    do {
    // bigFilePath = lastBigFilePath + id + string.format("_%06d", bfIndex) + ".bf";
    bigFilePath = lastBigFilePath + id + std::to_string(bfIndex) + ".bf";
    ++bfIndex;
    } while (false); // new File(bigFilePath).exists());
    bigfile* bigFile = new bigfile(reinterpret_cast<sdk::cores::chunk::chunkmanager*>(this), bigFilePath, allocBlocks, blockCount);
    bigfiles.emplace_back(bigFile);
    allocBlocks += blockCount;
  }
  totalblock = newTotalBlock;
  // bitFile.setLength(getBitFileSize());
  update_spaceinfo();

}
void chunk_manager::writeto_file(string filepath) {

}
void chunk_manager::readfrom_file(string filepath) {

}

char* chunk_manager::get_spaceinfo() {
  return writeToBytes();
}
void chunk_manager::update_spaceinfo() {
  writeto_file(filepath);
}
bigfile::blocktarget* chunk_manager::create_blocktarget(int blockindex) {
  for(list<unique_ptr<bigfile>>::iterator itr = bigfiles.begin(); itr != bigfiles.end(); ++itr) {
    bigfile::blocktarget* target = itr->get()->create_blocktarget(blockindex);
    if(target) { // create success, return 
      return target;
    }
    // continue;
  }
  return NULL;
}
bigfile* chunk_manager::get_bigfile(int index) {
  return bigfiles.front().get();
}



#ifndef TEST_MAIN
int main() {
  sdk::plugins::chunk_manager chunkmgr("path", 12*1024*1024, NULL);
  chunkmgr.plugin_startup();
}
#endif