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
    blocktarget();
    blocktarget(bigfile_ptr bf, int blockindex);
    ~blocktarget();

    int writedata(int offsetinblock, const char* data, int offset, int len);
    int readdata(int offsetinblock, char* data, int offset, int len);

    inline bigfile_ptr get_bigfile() { return this->bfile; }

    int64 block_startoffset_infile; // 
    bigfile_ptr bfile;
  };

  typedef bigfile::blocktarget blocktarget_type;
  typedef std::unique_ptr<blocktarget_type> blocktarget_ptr;

public:
  bigfile();
  bigfile(chunkmanager* manager, string filepath, int startindex, int blockcount);
  bigfile(chunkmanager* manager, instreamhelp& is);
  ~bigfile();

  /**
   * bigfile属性写入chunkmanager索引文件
   **/
  void writeinfo(outstreamhelp& os);

  /**
   * 创建bigfile的块元素
   **/
  blocktarget* create_blocktarget(int blockindex);

  inline int start_index() { return startindex; } // 本bigfile维护的索引位置chunkmanager中
  inline int get_blockcount() { return blockcount; } // 本bigfile维护的块数量
  int get_blocksize(); // chunkmanager块大小
  int64 get_size(); // chunkmanager整个文件大小
  bool valid(); // 文件有效

protected: 
  // 获取共享计数，不直接使用this
  bigfile_ptr self() {
    return bigfile::shared_from_this();
  }

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