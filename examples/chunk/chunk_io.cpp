
#include "cores/chunk/chunk.hpp"

using namespace sdk::cores::chunk;


int chunk_equal() {
  bool ret = false;
  chunk ck1("id1", 10);
  chunk ck2("id1", 100);
  chunk ck3;

  ret = (ck1 == ck2);
  std::cout << "chunk_io::" << __FUNCTION__ << ", return " << ret << std::endl;
  std::cout << ck1 << std::endl;
  ret = (ck1 == ck3);
  std::cout << "chunk_io::" << __FUNCTION__ << ", ck1 == ck3 before " << ret << std::endl;
  
  // 结构写文件，再读文件
  ofstream myfile ("example.txt");
  if (myfile.is_open())
  {
    // myfile << ck1;
    // ck1.write(myfile);
    myfile.close();
  }
  else cout << "Unable to open file" << endl;

  ifstream myfiler ("example.txt");
  if (myfiler.is_open())
  {
    std::cout << "chunk_io::" << __FUNCTION__ << " ck3 " << ck3 << std::endl;
    // myfiler >> ck3;
    // ck3.read(myfiler);
    std::cout << "chunk_io::" << __FUNCTION__ << " ck3 " << ck3 << std::endl;
    myfiler.close();
  }
  else cout << "Unable to open file"; 


  ret = (ck1 == ck3);
  std::cout << "chunk_io::" << __FUNCTION__ << ", ck1 == ck3 after " << ret << std::endl;
  return ret;
}
int main() {

  chunk_equal();
  return 0;
}