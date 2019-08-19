
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


long random_chunkid() {
  time_t timenow = chrono::system_clock::to_time_t(chrono::system_clock::now());
  return timenow;
}

class chunk_manager: public plugin<chunk_manager>, public streampecker
{
public:
  chunk_manager() = delete;
  ~chunk_manager() {
    std::cout << "chunk_manager::" << __FUNCTION__ << std::endl;
  }

  void plugin_initialize(const options* params) {
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

  // 获取update
  onupdate* on_update();
  bool is_intact();
  void flush_bits(list<chunk::stripinfo>& strips, bool value);
  inline string& get_id() { return id; }
  inline int64 get_totalspace() { return (int64) totalblock * blocksize; }
  inline int64 get_usedblock() { return usedblock; }
  inline int64 get_freespace() { return (int64)(totalblock - get_usedblock()) * blocksize; }
  inline int64 get_usedspace() { return (int64) get_usedblock() * blocksize; }
  inline int get_blocksize() { return blocksize; }
  inline int calc_blockcount(int chunksize) { return (chunksize + blocksize - 1) / blocksize; }
  inline const string& get_id() const { return id; }

  chunk* loadchunk(string chunkid, string chunkinfo);
  int find_onestrip(int blockcount);
  list<chunk::stripinfo>* alloc_blockstrips(int blockcount);
  void check_locks();
  chunk* alloc_chunk(string chunkid, int chunksize, int lifelen);
  void clear_lockedchunk();
  chunk* find(string chunkid);
  chunk* unlock_chunk(string chunkid);
  void release_chunk(string chunkid);
  void release_chunk(chunk* chunk);
  char* get_spaceinfo();
  void update_spaceinfo();
  bigfile::blocktarget* create_blocktarget(int blockindex);
  bigfile* get_bigfile(int index);


protected:
  void write(outstreamhelp &os);
  void read(instreamhelp &is);

  void resize(int sizemb);
  void writeto_file(string filepath);
  void readfrom_file(string filepath);
private:
  chunk_manager(string id, string cmpath, int sizemb, onupdate* update);
  chunk_manager(string id, string filepath, onupdate* update);

private:
  string filepath;
  string version;
  int blocksize;
  string id;
  int totalblock;
  int usedblock;
  
  // 索引文件,标记block是否被使用了
  string bitfilepath;
  fstream bitfile;
  std::unique_ptr<bitsetio> bsetio;

  list<std::unique_ptr<bigfile>> bigfiles;
  int blockcheckindex;
  std::unique_ptr<onupdate> update;

  list<std::unique_ptr<chunk>> lockedchunks;



  static string CHUNK_MANAGER_VERSION_1_0; // chunkmanager version 1.0 
  static string CHUNK_MANAGER_VERSION_1_1; // chunkmanager version 1.1
};

} /// namespace plugins
} /// namespace sdk

#endif