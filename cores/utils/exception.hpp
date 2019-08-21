

#ifndef SDK_CORES_UTILS_EXCEPTION_H
#define SDK_CORES_UTILS_EXCEPTION_H

#include <map>
#include <string>

namespace sdk {
namespace cores {
namespace utils {

enum class CodeException: int
{
  SYNCFILE_FILE_OPEN_FAILED = 1,
  SYNCFILE_FILE_SETLENGHT_FAILED,
  SIMPLEPROVE_SEED_ERROR,
  DYNMAIC_PTRNULL,
  FILE_READ_OP_ERROR,
  FILE_WRITE_OP_ERROR,
  FILE_PATH_NOT_EXIST,
  INDEX_OVER_REGION,
  FullMd5_CTX_RELEASE,
  FullMd5_UPDATE_SEGINDX_ERROR,
  FullMd5_SEGINDEX_ERROR,
  FullMd5_SEGCOUNT_NOTFILL,
  CHECK_MCHUNK_PARAM_ERROR,
};

enum class EncoderError : int
{
  INIT_PARAMS_ERROR = 0,
  INIT_FILE_ERROR,
  INIT_FILE_NOT_BIG_ENOUGH,
  INIT_CREATETMPFILE_ERROR,
  READ_FILE_ERROR,
  ENCRYPT_KEY_NULL,
  CHUNKINDEX_ERROR,
  OLD_M_FILE_ERROR,
  REMOVE_M_ERROR,
  JERASDENCODE_ERROR,
  SEGINDEX_ERROR,
  ENCODE_NOT_FINISHED_ERROR,
  M_CHUNK_MD5_NOT_CONSIST,
};

enum class DecoderError : int
{
  INIT_PARAMS_ERROR = 0,
  GET_SEGMENTFILE_NULL,
  BLOCK_OFFSET_ERROR,
  SEGMENTFILE_NULL,
  CONFIGINDEX_NULL,
  FILEDECODER_NULL,
  DECODE_UNNORMAL_STOP,
  ADDCHUNK_FULL_K,
  ADDCHUNK_ALWAYS_INDEX,
  WRITECHUNK_NOT_ADDCHUNK,
  JERASDECODE_ERROR,
  SEGMENT_CHUNKINDEX_OVERLOP,
  WRITECHUNK_FULL_DATA
};

enum class StreamError: int
{
  FILE_OPEN_ERROR = 3000,
  FILE_FDSOCK_ERROR,
  FILE_READWRITE_LEN_ERROR,

};

enum class ChunkError: int
{
  OFFSETINBLOCK_MAX = 4000,

};


enum error_code {

  // base
  STREAM_NOT_SUPPORT_NEGATIVE_NUMBER = 100100,
  STREAM_WRITE_FAILED,
  STREAM_READ_FAILED,
  STREAM_MAX_VALUE,

  // filestream
  FILE_OPEN_ERROR = 3000,
  FILE_FDSOCK_ERROR,
  FILE_READWRITE_LEN_ERROR,



  // bigfile 
  BIGFILE_PTR_RELEASED = 400100,
  BIGFILE_SIZE_FIXED,
  BIGFILE_VERSION_NOT_FIT,
  BIGFILE_NOT_EXIST,
  // bigfile::blocktarget
  BLOCKTARGET_NO_IN_BIGFILE = 400200,
  BLOCKTARGET_MAX_LIMIT,

  // chunk

  // chunkmanager
  CHUNKMANAGER_ID_NOT_FIX = 400300,
  CHUNKMANAGER_META_BREAKDOWN,
  CHUNKMANAGER_SIZE_MIN_LIMIT,
  CHUNKMANAGER_NEED_ONUPDATE,
  CHUNKMANAGER_NEED_SPACEPATH,

  // max -1
  ERR_CODE_GENERAL = -1,
};

extern std::map<int, std::string> err_messages;

class sdkexception: public std::exception
{
public:
  sdkexception(int code, std::string const &msg = "")
    :m_msg(msg), m_code(code) {}

  ~sdkexception() throw() {}

  virtual char const* what() const throw();
  inline int code() const throw() {  return m_code; }

  const std::string m_msg;
  int m_code;
};



} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {
  

#endif