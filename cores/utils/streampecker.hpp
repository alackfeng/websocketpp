
#ifndef SDK_CORES_UTILS_STREAMPECKER_H
#define SDK_CORES_UTILS_STREAMPECKER_H

#include <cores/utils/types.hpp>
#include "SyncFile.hpp"

namespace sdk {
namespace cores {
namespace utils {

class stream
{
public:
  virtual ~stream() {}
  
  // virtual friend ostream& operator<<(ostream &os, const int& ck) = 0;
  // virtual friend istream &operator>>(istream &is, int& ck) = 0;
  virtual void read() = 0;
  
};

class streaminput
{
public:
  streaminput();
  ~streaminput();
  
};

class instreamhelp: public SyncFile
{
public:
  instreamhelp(const char* filepath)
    :SyncFile(filepath, true) {}
  ~instreamhelp() {}
  
  int read_(void* b, int len);
  int read_(char* b, int off, int len);
  void readFully(char* b, int len);
  void readFully(char* b, int off, int len);
  int skipBytes(int n);
  bool readBoolean();
  char readByte();
  int readUnsignedByte();
  short readShort();
  int readUnsignedShort();
  char readChar();
  int readInt();
  long readLong();
  float readFloat();
  double readDouble();
  std::string readLine();
  std::string readUTF();
  std::string readUTF(instreamhelp& in);

private:
  char readBuffer[8] = {0};
  char bytearr[80] = {0};
  char chararr[80] = {0};
  int64 offset = 0;
};

class outstreamhelp: public SyncFile
{
public:
  outstreamhelp(const char* filepath)
    :SyncFile(filepath, false) {}
  ~outstreamhelp() {}

  void write_(int b);
  void write_(void *b, int off, int len);
  void flush();
  void writeBoolean(bool v);
  void writeByte(int v);
  void writeShort(int v);
  void writeChar(int v);
  void writeInt(int v);
  void writeLong(long v);
  void writeFloat(float v);
  void writeDouble(double v);
  void writeBytes(std::string s);
  void writeChars(std::string s);
  void writeUTF(std::string str);
  int writeUTF(std::string str, outstreamhelp* this_);
  inline int size() {  return written; }

private:

  inline void incCount(int value) {
      int64 temp = written + value;
      if (temp < 0) {
          throw 123; // 
          // temp = Integer.MAX_VALUE;
      }
      written = temp;
  }

  int64 written = 0; 
  char* bytearr = NULL;
  char writeBuffer[8] = {0};
};

class streampecker
{
public:
  virtual ~streampecker() {}
  virtual void read(instreamhelp& is) = 0;
  virtual void write(outstreamhelp& os) = 0;

  virtual void readFromBytes(char* buffer) {

  }

  virtual char* writeToBytes() {
    return NULL;
  }

  virtual void readFromFile(const string& filepath) {
    
    try {
      instreamhelp is(filepath.c_str());
      read(is);
    } catch(std::exception& e) {
      std::cout << "std::exception " << e.what() << std::endl;
    } catch(...) {
      std::cout << "std::fatal over." << std::endl;
    }
  }

  virtual void writeToFile(const string& filepath) {

    try {
      outstreamhelp os(filepath.c_str());
      write(os);
      os.flush();
    } catch(std::exception& e) {
      std::cout << "std::exception " << e.what() << std::endl;
    } catch(...) {
      std::cout << "std::fatal over." << std::endl;
    }
  }
  
};

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {

#endif