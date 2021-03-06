
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
  explicit instreamhelp(const char* filepath)
    :SyncFile(filepath, true) {}
  explicit instreamhelp(const string& filepath)
    :instreamhelp(filepath.c_str()) {}
  ~instreamhelp() {}
  
  int read_(void* b, int len);
  int read_(unsigned char* b, int off, int len);
  void readFully(unsigned char* b, int len);
  void readFully(unsigned char* b, int off, int len);
  int skipBytes(int n);
  bool readBoolean();
  unsigned char readByte();
  int readUnsignedByte();
  short readShort();
  int readUnsignedShort();
  char readChar();
  int readInt(); // 支持负数的
  long readLong();
  float readFloat();
  double readDouble();
  std::string readLine();
  std::string readUTF();
  std::string readUTF(instreamhelp& in);

private:

  inline void incCount(int value) {
      int64 temp = offset + value;
      if (temp < 0) {
          throw 123; // 
          // temp = Integer.MAX_VALUE;
      }
      offset = temp;
  }

  unsigned char readBuffer[8] = {0};
  char bytearr[80] = {0};
  char chararr[80] = {0};
  int64 offset = 0;
};

class outstreamhelp: public SyncFile
{
public:
  explicit outstreamhelp(const char* filepath)
    :SyncFile(filepath, false) {}
  explicit outstreamhelp(const string& filepath)
    :outstreamhelp(filepath.c_str()) {}
  ~outstreamhelp() {}

  void write_(unsigned char b);
  void write_(void *b, int off, int len);
  void flush();
  void writeBoolean(bool v);
  void writeByte(int v); // 8 bytes
  void writeShort(int v);
  void writeChar(int v); // 16 bytes
  void writeInt(int v);
  void writeLong(long v);
  void writeFloat(float v);
  void writeDouble(double v);
  void writeBytes(const std::string& s); // string
  void writeChars(const std::string& s);
  void writeUTF(const std::string& str);
  int writeUTF(const std::string& str, outstreamhelp& os);
  inline int size() {  return written; }

private:
  void incCount(int value);

  int64 written = 0; 
  char* bytearr = NULL;
  unsigned char writeBuffer[8] = {0};
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
      std::cout << "readFromFile std::exception " << e.what() << std::endl;
    } catch(...) {
      std::cout << "readFromFile std::fatal over." << std::endl;
    }
  }

  virtual void writeToFile(const string& filepath) {

    try {
      outstreamhelp os(filepath.c_str());
      write(os);
      os.flush();
    } catch(std::exception& e) {
      std::cout << "writeToFile std::exception " << e.what() << std::endl;
    } catch(...) {
      std::cout << "writeToFile std::fatal over." << std::endl;
    }
  }
  
};

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {

#endif