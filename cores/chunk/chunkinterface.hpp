#ifndef SDK_CORES_CHUNK_DATABASE_H
#define SDK_CORES_CHUNK_DATABASE_H

#include <list>
#include <string>
#include <cores/chunk/chunk.hpp>
#include <cores/chunk/bigfile.hpp>

namespace sdk {
namespace cores {
namespace chunk {

class onupdate
{
public:
  // delete called on 'onupdate' that is abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
  virtual ~onupdate() {}
  // 获取可用空间的路径
  virtual string getSpacePath(int64 spaceSize) = 0;
  // 更新保存ChunkManager属性信息
  virtual bool onUpdateChunk(string chunkID, string chunkInfo); // couchDB启动时要维护
  // 更新保存Chunk属性信息
  virtual bool onReleaseChunk(string chunkID); // couchDB启动时要维护
  //
  virtual void onSpaceCheckError(int actualMB);

};

using bigfile_ptr = bigfile::bigfile_ptr;
using blocktarget_type = bigfile::blocktarget_type;
using stripinfo_type = chunk::stripinfo_type;
class chunkmanager
{
public:
  virtual ~chunkmanager() {}
  virtual int calc_blockcount(int chunksize) = 0;
  virtual blocktarget_type* create_blocktarget(int blockindex) = 0;
  virtual list<stripinfo_type>* alloc_blockstrips(int blockcount) = 0;
  virtual bigfile_ptr get_bigfile(int index) = 0;
  virtual const string& get_id() const = 0;
  virtual int get_blocksize() = 0;
  virtual onupdate* on_update() = 0;
};

typedef enum SelfCheckState
{
  CHECKING = 0,   // 开始自检
  DIFFUSEING  // 开始扩散 
}SelfCheckState;

typedef struct ChunkInfo
{
  std::string      chunkId;      // String
                            // chunk的ID
                            // 文件的chunkId组成VSPId+visitId+fileId+index生成的Md5;
                            // 文件列表chunkId
                            // vspId+visitorId生成的Md5
  std::list<std::string>  saveDevices;                // json字符串
                            // 存储碎片的设备列表
  long             checkTime;                  // long 碎片自检时间
  std::string      publicKey;                  // String 公钥
  int              ChunkIndex    = 0;            // int 文件索引
  std::string      ResHash;                  // String 文件Id
  std::string      chunkType;                  // String 碎片类型
  std::string      chunkInfo;                  // String
                            // 碎片manager中的一个索引
  std::string      version;                  // String 文件列表版本
  std::string      token;                    // String token值
  int              chunkSize;                  // 片大小
  int              chunkSafeInfo;                // 存储,k,m,copycount信息
  long             deleteTime;                 // 删除时间
  SelfCheckState   selfCheckState  = CHECKING;  // 自检状态
  std::string      DomainName;                 // 所在的域
  int              startOffse;                 // 下载的起始位置
  int              TaskStatus;                 // chunk任务状态
                                        // 任务状态
                                        // 1、Compalte
                                        // 2、Execution
  std::string      DataVerify;                // 数据指纹
  std::string      OldToken;                 // 原始token

}ChunkInfo;

class database
{
public:

  virtual bool Init() = 0;
  // 删除碎片信息到数据库
  virtual bool DeleteChunk(std::string chunkid) = 0;
  // 创建碎片信息到数据库（chuankManager锁定时写入）
  virtual bool UpdateChunkInfo(std::string chunkid, std::string chunkinfo) = 0;
  // 更新check时间
  virtual bool UpdateChunkInfo(std::string chunkid, std::list<std::string> devices, long checktime) = 0;
  // 更新check时间
  virtual bool UpdateChunkInfo(std::string chunkid, long checktime) = 0;
  // 更新check时间
  virtual bool UpdateChunkInfo(std::string chunkid, int taskstatus) = 0;
  // 更新资源设备
  virtual bool UpdataDeviceList(std::string chunkid, std::list<std::string> devices) = 0;
  // 更新碎片基础信息（p2p接收完碎片时写入
  virtual bool CreateChunkInfo(ChunkInfo chunkInfo) = 0;
  // 读取碎片信息
  virtual ChunkInfo ReadChunkInfo(std::string ResHash, int ChunkIndex) = 0;
  // 读取扩散碎片
  virtual ChunkInfo ReadOldestChunkInfo() = 0;
  // 读取扩散碎片
  virtual ChunkInfo ReadChunkInfo() = 0;
  // 
  virtual ChunkInfo ReadOldestFileList() = 0;
  // 读取多个碎片
  virtual std::list<ChunkInfo> ReadChunkInfos(std::string ResHash) = 0; 
  // 更新文件列表信息 
  virtual bool UpdataFileListInfo(std::string ChunkID, std::string Version) = 0;
  // 保存碎片管理器路
  virtual bool SaveChunkManagerPath(std::string path) = 0;
  // 返回碎片管理
  virtual std::string GetChunkManagerPath() = 0;
  // 删除路径
  virtual void DeleteChunkManagerPath() = 0;
  // 添加删除标记
  virtual bool UpdateDeleteFlag(std::string ChunkID, long deleteTime) = 0;
  // 删除所有RD数据
  virtual void deleteAllData() = 0;
};

} /// namespace chunk {
} /// namespace cores {
} /// namespace sdk {

#endif