#ifndef SDK_CORES_CHUNK_BIGFILE_H
#define SDK_CORES_CHUNK_BIGFILE_H

#include <cores/utils/types.hpp>
#include <cores/utils/fileio.hpp>
#include <cores/utils/SyncFile.hpp>
#include <cores/utils/streampecker.hpp>
using namespace sdk::cores::utils;

namespace sdk {
namespace cores {
namespace chunk {

using namespace std;
class chunkmanager;
class bigfile: public SyncFile
{
public:
  class blocktarget
  {
  public:
    explicit blocktarget() {}
    explicit blocktarget(bigfile* bf, int blockindex): bfile(bf) {
      startindex = get_bigfile()->startindex;
      int64 internalindex = blockindex - startindex;
      block_startoffset_infile = internalindex * startindex;
    }
    ~blocktarget() {
    }

    int writedata(int offsetinblock, const char* data, int offset, int len);
    int readdata(int offsetinblock, char* data, int offset, int len);

    inline bigfile* get_bigfile() { return this->bfile; }

    int startindex;
    int block_startoffset_infile;
    bigfile* bfile;
  };

public:
  // bigfile() {}
  bigfile(chunkmanager* manager, string filepath, int startindex, int blockcount);
  bigfile(chunkmanager* manager, instreamhelp& is);
  ~bigfile();

  void writeinfo(outstreamhelp& os);

  /**
   * 
   **/
  blocktarget* create_blocktarget(int blockindex);

  inline int start_index() { return startindex; }
  inline bool data() { return true; }
  inline int get_blockcount() { return blockcount; }
  int get_blocksize();
  int64 get_size();

private:
  string version;         // 当前版本
  chunkmanager* manager;  // chunk管理器
  int startindex;         // 在总空间的开始位置
  int blockcount;         // 可存储的块数量
  string filepath;        // 大文件路径
  fstream fsbfile;        // 文件操作

  static string BIG_FILE_VERSION; // bigfile version
};

} /// namespace chunk {
} /// namespace cores {
} /// namespace sdk {
  
#endif