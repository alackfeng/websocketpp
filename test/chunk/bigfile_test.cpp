
//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE bigfile_test
#include <boost/test/unit_test.hpp>
#include <iostream>
using namespace std;

#include <cores/chunk/chunkinterface.hpp>
#include <cores/chunk/bigfile.hpp>
using namespace sdk::cores::chunk;

class chunkmanager_implt: public chunkmanager
{
public:
  chunkmanager_implt() {}
  virtual ~chunkmanager_implt() {}
  virtual int calc_blockcount(int chunksize) {
    return 0;
  }
  virtual blocktarget_type* create_blocktarget(int blockindex) {
    return NULL;
  }
  virtual list<stripinfo_type>* alloc_blockstrips(int blockcount) {
    list<stripinfo_type> strips;
    return &strips;
  }
  virtual bigfile_ptr get_bigfile(int index) {
    return NULL;
  }
  virtual const string& get_id() const {
    static std::string id("");
    return id;
  }
  virtual int get_blocksize() {
    return 0;
  }
  virtual onupdate* on_update() {
    return NULL;
  }
  
};

std::shared_ptr<chunkmanager> create_chunkmanager() {
  std::shared_ptr<chunkmanager> ckm = std::make_shared<chunkmanager_implt>();
  return ckm;
}

BOOST_AUTO_TEST_CASE( bigfile_test ) {
  BOOST_CHECK( true );
  // BOOST_ERROR( "test error");
  // BOOST_CHECK_MESSAGE( true, "ok" );
  // BOOST_CHECK_EQUAL("ok", "ok");
  // std::cout << "bigfile_test" << std::endl;
}

// 测试bigfile shared_ptr 引用计数是否正常
BOOST_AUTO_TEST_CASE( bigfile_test_shared_ptr_used ) {
  // BOOST_CHECK( true );

  // loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;
  std::string filepath = "./chunk_data.txt";
  int startindex = 0;
  int blockcount = 10;
  chunkmanager_implt* impl = NULL; // new chunkmanager_implt();
  std::shared_ptr<chunkmanager> ckm = create_chunkmanager();
  BOOST_CHECK_MESSAGE(ckm.use_count() == 1, "ckm ptr count not equal 1");
  std::shared_ptr<bigfile> sBigfile = std::make_shared<bigfile>(ckm.get(), filepath, startindex, blockcount);
  BOOST_CHECK_MESSAGE(ckm.use_count() == 1, "ckm ptr count not equal 1 .");
  BOOST_CHECK_MESSAGE(sBigfile.use_count() == 1, "bigfile ptr count not equal 1.");
  
  int blockindex = 0;
  blocktarget_type* ptarget = sBigfile->create_blocktarget(blockindex);
  BOOST_CHECK_MESSAGE(ptarget, "ptarget is NULL.");
  BOOST_CHECK_MESSAGE(sBigfile.use_count() == 2, "bigfile ptr count not equal 2.");

  std::shared_ptr<bigfile> bfileo = ptarget->get_bigfile();
  BOOST_CHECK_MESSAGE(sBigfile.use_count() == 3, "bigfile ptr count not equal 3.");

  sBigfile.reset();
  BOOST_CHECK_MESSAGE(bfileo.use_count() == 2, "bfileo ptr count not equal 2.");
  BOOST_CHECK_MESSAGE(sBigfile.use_count() == 0, "bigfile ptr count not equal 0.");
  BOOST_CHECK_MESSAGE(sBigfile == NULL, "sBigfile reset , now null.");

  if(sBigfile) {
    BOOST_CHECK_MESSAGE(false, "sBigfile reset , now null.");
    sBigfile->create_blocktarget(1);  
  } 


}




