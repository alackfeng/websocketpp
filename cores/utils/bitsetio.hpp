#ifndef SDK_CORES_UTILS_BITSETIO_H
#define SDK_CORES_UTILS_BITSETIO_H

#include <cores/utils/types.hpp>

#ifndef BITSIZE
#define BITSIZE(sz) (((sz) + 7) / 8)
// #define BITSIZE(sz) (((sz) >> 3) + 1)
#endif
using namespace std;


namespace sdk {
namespace cores {
namespace utils {


class bitsetio
{
public:
  bitsetio(int size): size_(size) {}
  ~bitsetio() {}
  
  // 获取bitset大小
  inline int size() {
    return BITSIZE(size_);
  }

  // 为1的数量
  inline int count() {
    return 11;
  }

  inline void set(int index) {
    
  }

  inline bool check(int block) {
    if (count() != block) return false;
    return true;
  }

  bool init_bitfile(const string& filepath);
  bool load_bitfile(const string& filepath);
  bool set_bitfile(int startindex, int endindex, bool value); 

private:
  int size_;

  string filepath;
  fstream bffile;
};

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {

#endif