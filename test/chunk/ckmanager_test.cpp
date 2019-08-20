
//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ckmanager_test
#include <boost/test/unit_test.hpp>
#include <cores/utils/types.hpp>
#include <plugins/chunk_manager/chunk_manager.hpp>
#include <plugins/base/plugin_manage.hpp>
using namespace sdk::plugins;
using namespace sdk::cores::chunk;


class ckmanager_update: public onupdate
{
public:
  ckmanager_update() {}
  virtual ~ckmanager_update() {}

    // 获取可用空间的路径
  virtual string getSpacePath(int64 spaceSize) override {
    return path;
  }
  // 更新保存ChunkManager属性信息
  virtual bool onUpdateChunk(string chunkID, string chunkInfo) override {
    return true;
  }
  // 更新保存Chunk属性信息
  virtual bool onReleaseChunk(string chunkID) override {
    return true;
  }
  //
  virtual void onSpaceCheckError(int actualMB) override {
    return;
  }
  
  string path = "./data/";
};

BOOST_AUTO_TEST_CASE( ckmanager_test_create ) {
  BOOST_CHECK( true );

  string cmpath("./data/");
  int sizemb = MIN_BIG_FILE_SIZE;
  onupdate* update = new ckmanager_update();
  auto plugin_ckmanager = sdk::plugins::base::plugins().register_plugin<chunk_manager>(cmpath, sizemb, update);
  plugin_ckmanager->plugin_initialize();
  plugin_ckmanager->plugin_startup();
}