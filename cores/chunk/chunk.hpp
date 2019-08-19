#ifndef SDK_CORES_CHUNK_CHUNK_H
#define SDK_CORES_CHUNK_CHUNK_H

#include <cores/utils/types.hpp>
#include <cores/utils/streampecker.hpp>
#include <cores/chunk/bigfile.hpp>

namespace sdk {
namespace cores {
namespace chunk {

using namespace std;


using blocktarget_type = bigfile::blocktarget_type;
class chunk: public streampecker
{
public:
  class stripinfo
  {
  public:
    stripinfo(int startindex, int blockcount)
      :startindex(startindex), blockcount(blockcount) {}
    ~stripinfo() {}
    inline int startpos(int blocksize) {
      return startindex * blocksize;
    }
    inline int endpos(int blocksize) {
      return (startindex + blockcount) * blocksize;
    }
    int startindex;
    int blockcount;
  };

  typedef chunk::stripinfo stripinfo_type;

public:
  chunk(const string& id, const int& cksize)
    : chunkid(id), chunksize(cksize), datasize(-1), currentoffset(8888), deadtime(0) {}
  chunk(chunkmanager* manager);
  chunk(chunkmanager* manager, instreamhelp& is);
  chunk() {}
  ~chunk();

  chunk* init(string chunkid, int chunksize);
  chunk* loadfrom(char* chunkinfo);

  friend bool operator==(const chunk& ck, const chunk& other);
  // write or read
  friend ostream& operator<<(ostream &os, const chunk& ck);
  friend istream &operator>>(istream &is, chunk& ck);

  void write(outstreamhelp &os);
  void read(instreamhelp &is);

  inline string& get_chunkid() { return chunkid; }
  inline int get_chunksize() { return chunksize; }
  inline int get_datasize() { return datasize; }
  int get_blockcount();
  inline chunk* addstrip(int startindex, int blockcount) {
    strips.emplace_back(stripinfo(startindex, blockcount));
    return this;
  }
  inline chunk* addstrips(list<stripinfo> &stripsl) {
    return this;
  }
  inline list<stripinfo>& getstrips() { return strips; }
  inline chunk* set_lifelen(int64 lifelen) { 
    return this;
  }
  inline bool is_alive(int64 now) {
    return true;
  }
  inline void writeinfo(outstreamhelp& os) {
    write(os);
  }
  inline string get_chunkinfo() {
    char* bytes = writeToBytes();
    return bytes;
  }

  void extract_blocktargets();
  int writedata(int offsetinchunk, char* data, int len);
  int readdata(int offsetinchunk, char* data, int len);

  inline bool seek(int offset) {
    if (offset < 0 || offset > chunksize) {
      return false;
    }
    currentoffset = offset;
    return true;
  }
  inline void seek_toend() {
    currentoffset = chunksize;
  }
  inline int writedata(char* data, int len) {
    int writesize = writedata(currentoffset, data, len);
    return currentoffset += writesize;
  }
  inline int readdata(char* data, int len) {
    int readsize = readdata(currentoffset, data, len);
    return currentoffset += readsize;
  }

  inline char* get_checkbytes(int len, int seed) {
    return NULL;
  }
  inline char* get_md5() {
    return NULL;
  }



  
private:
  chunkmanager* manager; // 
  string chunkid;
  int chunksize;
  int datasize = 0;
  int currentoffset = 0;
  list<stripinfo> strips;
  int deadtime = 0;
  bool locked = false;
  blocktarget_type* blocktargets = NULL;

  static string CHUNK_VERSION; // chunk version
};

} /// namespace chunk {
} /// namespace cores {
} /// namespace sdk {

#endif