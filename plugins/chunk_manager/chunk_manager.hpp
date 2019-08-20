
#ifndef SDK_PLUGINS_CHUNK_MANAGER_H
#define SDK_PLUGINS_CHUNK_MANAGER_H

#include <iostream>
#include <cores/utils/types.hpp>
#include <cores/utils/bitsetio.hpp>
#include <cores/chunk/chunkinterface.hpp>
#include <plugins/base/plugin.hpp>
using namespace std;
using namespace sdk::plugins::base;
using namespace sdk::cores::chunk;
using namespace sdk::cores::utils;

namespace sdk {
namespace plugins {


inline long random_chunkid() {
  time_t timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
  return timenow;
}

class chunk_manager: public plugin<chunk_manager>, public streampecker, public chunkmanager
{
public:
  chunk_manager() = delete;
  ~chunk_manager() {
    std::cout << "chunk_manager::" << __FUNCTION__ << std::endl;
  }

  void plugin_initialize(const options* params = NULL) {
    std::cout << "chunk_manager::" << __FUNCTION__ << std::endl;
  }
  void plugin_startup() {
    std::cout << "chunk_manager::" << __FUNCTION__ << std::endl;
  }
  void plugin_shutdown() {
    std::cout << "chunk_manager::" << __FUNCTION__ << std::endl;
  }

  chunk_manager(string cmpath, int sizemb, onupdate* update)
    :chunk_manager(std::to_string(random_chunkid()), cmpath, sizemb, update) {}
  chunk_manager(string filepath, onupdate* update)
    :chunk_manager(string(""), filepath, update) {}


  bool is_intact();
  void flush_bits(list<chunk::stripinfo>& strips, bool value);
  inline int64 get_totalspace() { return (int64) totalblock * blocksize; }
  inline int64 get_usedblock() { return usedblock; }
  inline int64 get_freespace() { return (int64)(totalblock - get_usedblock()) * blocksize; }
  inline int64 get_usedspace() { return (int64) get_usedblock() * blocksize; }


  // impliment chunkmanager
  inline int calc_blockcount(int chunksize) override { return (chunksize + blocksize - 1) / blocksize; }
  blocktarget_type* create_blocktarget(int blockindex) override;
  list<stripinfo_type>* alloc_blockstrips(int blockcount) override;
  bigfile_ptr get_bigfile(int index) override;
  inline const string& get_id() const override { return id; }
  inline int get_blocksize() override { return blocksize; }
  onupdate* on_update() override; // 获取update


  chunk* loadchunk(string chunkid, string chunkinfo);
  int find_onestrip(int blockcount);

  void check_locks();
  chunk* alloc_chunk(string chunkid, int chunksize, int lifelen);
  void clear_lockedchunk();
  chunk* find(string chunkid);
  chunk* unlock_chunk(string chunkid);
  void release_chunk(string chunkid);
  void release_chunk(chunk* chunk);
  char* get_spaceinfo();
  void update_spaceinfo();



protected:

  // impliment streampecker 
  void write(outstreamhelp &os) override;
  void read(instreamhelp &is) override;

  void resize(int sizemb);
  
private:
  chunk_manager(string id, string cmpath, int sizemb, onupdate* update);
  chunk_manager(string id, string filepath, onupdate* update);


private:
  string filepath;  // cm文件路径
  string version;   // cm版本
  int blocksize;    // 元数据大小
  string id;        // chunkmanager ID
  int totalblock;   // 支撑的元数据块个数
  int usedblock;    // 已经使用数量
  
  // 索引文件,标记block是否被使用了
  string bitfilepath;
  fstream bitfile;
  std::unique_ptr<bitsetio> bsetio;

  // bigfile指针需要共享
  list<bigfile_ptr> bigfiles; 
  int blockcheckindex;
  std::unique_ptr<onupdate> update;

  // 
  list<std::unique_ptr<chunk>> lockedchunks;

  static string CHUNK_MANAGER_VERSION_1_0; // chunkmanager version 1.0 
  static string CHUNK_MANAGER_VERSION_1_1; // chunkmanager version 1.1
};

} /// namespace plugins
} /// namespace sdk

#endif