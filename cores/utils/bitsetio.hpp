#ifndef SDK_CORES_UTILS_BITSETIO_H
#define SDK_CORES_UTILS_BITSETIO_H

#include <cores/utils/types.hpp>
#include <cores/utils/SyncFile.hpp>
using namespace sdk::cores::utils;

#ifndef BITSIZE
// #define BITSIZE(sz) (((sz) + 7) / 8)
#define BITSIZE(sz) (((sz) >> 3) + 1)
#endif
using namespace std;


namespace sdk {
namespace cores {
namespace utils {


class bitsetio
{
public:
  bitsetio() = default;
  bitsetio(const int64 size, const string& filepath = "");
  ~bitsetio();

  bitsetio(const bitsetio& rhs);
  bitsetio& operator=(const bitsetio& rhs);

  void set(const int64 position);
  int64 size() const;
  bool test(const int64 position) const;
  void reset(const int64 position);
  int64 count();


  inline bool check(int block) {
    if (count() != block) return false;
    return true;
  }

  bool init_bitfile();
  bool load_bitfile();
  bool ext_bitfile(const int64 size); // 扩展大小
  bool set_bitfile(int64 startindex, int64 endindex, bool value); 

private:
  unsigned char* bits_ = NULL;
  int64 allocated_ = 0;
  int64 size_ = 0;

  string filepath = "";
  SyncFile* bitfile = NULL;
};

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {

#endif