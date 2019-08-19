
#include "fileio.hpp"

namespace sdk {
namespace cores {
namespace utils {


typedef struct datarw
{
  int iA;
  string sB;
  char cC[16];
}datarw_t;

int test_fstream() {
  streampos size;
  char * memblock;

  ifstream file ("Makefile", ios::in|ios::binary|ios::ate);
  if (file.is_open())
  {
    size = file.tellg();
    memblock = new char [size];
    cout << "file size: " << size << endl;
    file.seekg (0, ios::beg);
    file.read (memblock, size);
    file.close();

    cout << "the entire file content is in memory - " << memblock << endl;

    delete[] memblock;
  }
  else cout << "Unable to open file";
  return 0;

}
int test_ifstream() {
  string line;
  ifstream myfile ("example.txt");
  if (myfile.is_open())
  {
    datarw_t data;
    myfile.read(reinterpret_cast<char*>(&data), sizeof(datarw_t));
    // myfile >> data;
    // while ( getline (myfile,line) )
    {
      cout << data.iA << '\n';
      cout << data.sB << '\n';
      cout << data.cC << '\n';
    }
    myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}

int test_ofstream() {
  ofstream myfile ("example.txt");
  if (myfile.is_open())
  {
    datarw_t data = datarw_t{.iA = 2, .sB = "ABC", .cC = "DEF"};
    myfile.write(reinterpret_cast<char*>(&data), sizeof(datarw_t));
    // myfile << reinterpret_cast<char*>(&data);
    // myfile << "\n";
    myfile.close();
  }
  else cout << "Unable to open file" << endl;
  return 0;
}

fileio::fileio(const char* filename, bool readonly)
:filename(filename), readonly(readonly), fs(filename, std::ios::in) {
  std::cout << "fileio:" << __FUNCTION__ << " filename " << filename << ", readonly " << readonly << std::endl;

  open(filename, readonly);
}


fileio::~fileio() {

  fs.close();
}

void fileio::open(const char* filename, bool readonly) {

  this->filename = filename;
  this->readonly = readonly;

  ios_base::openmode mode = readonly ? ios::binary|ios::out|ios::in : ios::binary|ios::in;
  fs.open(filename, mode);
  if (!fs.is_open()) {
    if (readonly) {
      throw 143; // 打开只读文件失败
    } else
      throw 144; // 文件未打开或创建
  }

  // 获取文件大小
  fs.seekg(0, ios::end);
  this->filesize = fs.tellg();

}

int64 fileio::setLength(int64 fileSize) {
  if (fileSize == filesize) 
    return filesize;
  if(!fs.is_open()) { // 文件未打开
    throw 154;
  }
  if(readonly) { // 只读文件不需要设置
    throw 157; // 
  }

  fs.seekg(fileSize - 1, ios::cur);
  fs.write(0, sizeof(char));
  return 0;
}

inline int64 fileio::getLength() {
  fs.seekg(0, ios::end);
  return fs.tellg();
}

int64 fileio::read(int64 fileOffset, void* buffer, int len) {
  fs.seekg(fileOffset, ios::cur);
  fs.read((char*)buffer, len);
  if(len != fs.gcount()) { // 写0
    int remain = len = fs.gcount();
    memset((char*)buffer + fs.gcount(), 0, remain);
  }
  return 0;
}

int64 fileio::write(int64 fileOffset, const void* buffer, int len) {
  fs.seekg(fileOffset, ios::cur);
  fs.write((char*)buffer, len);
  return fs.gcount();
}

void fileio::close() {
  fs.close();
}
void fileio::deleteFile() {

}
int fileio::truncFile(int64 len) {

}


} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {