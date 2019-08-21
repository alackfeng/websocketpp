
//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE streamhelp_test
#include <boost/test/unit_test.hpp>
#include <cores/utils/streampecker.hpp>
using namespace sdk::cores::utils;


#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)

BOOST_AUTO_TEST_CASE( streamhelp_test ) {

  const std::string filepath = "./stream.txt";
  outstreamhelp os(filepath);

  // int
  int blockcount = 10;
  os.writeInt(-1);
  os.writeInt(0);
  os.writeInt(INT_MAX);
  os.writeInt(INT_MIN);
  os.writeInt(blockcount);
  os.writeInt(blockcount + 65536);

  // long
  os.writeLong(-1);
  os.writeLong(0);
  long maxlong = std::numeric_limits<long>::max();
  os.writeLong(maxlong);
  long minlong = std::numeric_limits<int64>::min();
  os.writeLong(minlong);
  os.writeLong(DEFAULT_BIG_FILE_SIZE);
  long startindex = 1024;
  os.writeLong(startindex);
  os.writeLong(startindex + 65536);

  // bool
  bool bstart = true;
  os.writeBoolean(bstart);
  os.writeBoolean(false);
  os.writeBoolean(1);
  os.writeBoolean(0);

  // UTF
  os.writeUTF("ABC");
  char* p = new char[10];
  memset(p, 'A', 5);
  std::string sStr = "Hello World";
  sStr += "123";
  sStr += p;
  sStr = "CDG";
  os.writeUTF(sStr);

  os.flush();
  instreamhelp is(filepath);


  BOOST_CHECK_EQUAL(is.readInt(), -1);
  BOOST_CHECK_EQUAL(is.readInt(), 0);
  BOOST_CHECK_EQUAL(is.readInt(), INT_MAX);
  BOOST_CHECK_EQUAL(is.readInt(), INT_MIN);
  BOOST_CHECK_EQUAL(is.readInt(), blockcount);
  BOOST_CHECK_EQUAL(is.readInt(), blockcount + 65536);

  BOOST_CHECK_EQUAL(is.readLong(), -1);
  BOOST_CHECK_EQUAL(is.readLong(), 0);
  BOOST_CHECK_EQUAL(is.readLong(), maxlong);
  BOOST_CHECK_EQUAL(is.readLong(), minlong);
  BOOST_CHECK_EQUAL(is.readLong(), DEFAULT_BIG_FILE_SIZE);
  BOOST_CHECK_EQUAL(is.readLong(), startindex);
  BOOST_CHECK_EQUAL(is.readLong(), startindex + 65536);

  BOOST_CHECK_EQUAL(is.readBoolean(), bstart);
  BOOST_CHECK_EQUAL(is.readBoolean(), false);
  BOOST_CHECK_EQUAL(is.readBoolean(), 1);
  BOOST_CHECK_EQUAL(is.readBoolean(), 0);

  BOOST_CHECK_EQUAL(is.readUTF(), "ABC");
  BOOST_CHECK_EQUAL(is.readUTF(), sStr);


  std::cout << sizeof(int) << std::endl;
  std::cout << sizeof(long) << std::endl;
  std::cout << sizeof(long long) << std::endl;
  std::cout << std::numeric_limits<long>::max() << std::endl;
  std::cout << std::numeric_limits<long>::min() << std::endl;
  std::cout << std::numeric_limits<int>::max() << std::endl;
  std::cout << std::numeric_limits<int>::min() << std::endl;
  std::cout << std::numeric_limits<int64>::max() << std::endl;
  std::cout << std::numeric_limits<int64>::min() << std::endl;

  BOOST_CHECK( true );
}