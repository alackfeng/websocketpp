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
class bigfile: public SyncFile, public std::enable_shared_from_this<bigfile>
{
public:
  typedef std::shared_ptr<bigfile> bigfile_ptr;

  // 大文件中块处理
  class blocktarget
  {
  public:
    explicit blocktarget(): block_startoffset_infile(-1) {}
    explicit blocktarget(bigfile_ptr bf, int blockindex): bfile(bf) {
      int startindex = bfile->startindex;
      block_startoffset_infile = (int64)(blockindex - startindex) * bfile->get_blocksize();
    }
    ~blocktarget() {
    }

    int writedata(int offsetinblock, const char* data, int offset, int len);
    int readdata(int offsetinblock, char* data, int offset, int len);

    inline bigfile_ptr get_bigfile() { return this->bfile; }

    int block_startoffset_infile; // 
    bigfile_ptr bfile;
  };

  typedef bigfile::blocktarget blocktarget_type;

public:
  bigfile() {}
  bigfile(chunkmanager* manager, string filepath, int startindex, int blockcount);
  bigfile(chunkmanager* manager, instreamhelp& is);
  ~bigfile();

  bigfile_ptr self() {
    return bigfile::shared_from_this();
  }

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