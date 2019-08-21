#include "streampecker.hpp"

namespace sdk {
namespace cores {
namespace utils {


void outstreamhelp::write_(unsigned char b) {
  LOG_F(INFO, "outstreamhelp::write_ - written %lld, int %d.", written, b);
  int writelen = write(written, &b, 1);
  if(writelen != 1) {
    LOG_F(ERROR, "outstreamhelp::write_ - unsigned char written %lld, failed.", written);
    throw sdkexception(STREAM_WRITE_FAILED);
  }
  incCount(1);
}
void outstreamhelp::write_(void *b, int off, int len)
{
  LOG_F(INFO, "outstreamhelp::write_ - written %lld, off %d, len %d, %p.", written, off, len, (unsigned char*)b+off);
  int writelen = this->write(written, (unsigned char*)b+off, len);
  if(writelen != len) {
    LOG_F(ERROR, "outstreamhelp::write_ - void* written %lld, failed.", written);
    throw sdkexception(STREAM_WRITE_FAILED);
  }
  incCount(len);
}

void outstreamhelp::flush() {
  this->flushFile();
}

void outstreamhelp::writeBoolean(bool v) {
  this->write_(v ? 1 : 0);
  // incCount(1);
}

void outstreamhelp::writeByte(int v) {
  this->write_(v);
  // incCount(1);
}

void outstreamhelp::writeShort(int v) {
  this->write_((v >> 8) & 0xFF);
  this->write_((v >> 0) & 0xFF);
  // incCount(2);
}

void outstreamhelp::writeChar(int v) {
  this->write_((v >> 8) & 0xFF);
  this->write_((v >> 0) & 0xFF);
  // incCount(2);
}

void outstreamhelp::writeInt(int v) {
  // LOG_F(INFO, "outstreamhelp::writeInt - INT %d.", v);
  this->write_((v >> 24) & 0xFF);
  this->write_((v >> 16) & 0xFF);
  this->write_((v >>  8) & 0xFF);
  this->write_((v >>  0) & 0xFF);
  // incCount(4);
}

void outstreamhelp::writeLong(long v) {
  memset(writeBuffer, 0, 8);
  writeBuffer[0] = (unsigned char)(v >> 56);
  writeBuffer[1] = (unsigned char)(v >> 48);
  writeBuffer[2] = (unsigned char)(v >> 40);
  writeBuffer[3] = (unsigned char)(v >> 32);
  writeBuffer[4] = (unsigned char)(v >> 24);
  writeBuffer[5] = (unsigned char)(v >> 16);
  writeBuffer[6] = (unsigned char)(v >>  8);
  writeBuffer[7] = (unsigned char)(v >>  0);
  this->write_(writeBuffer, 0, 8);
  // incCount(8);
}

void outstreamhelp::writeFloat(float v) {
  // writeInt(Float.floatToIntBits(v));
  LOG_F(ERROR, "outstreamhelp::writeFloat - not implement.");
  assert(false);
}
void outstreamhelp::writeDouble(double v) {
  // writeLong(Double.doubleToLongBits(v));
  LOG_F(ERROR, "outstreamhelp::writeDouble - not implement.");
  assert(false);
}

void outstreamhelp::writeBytes(const std::string& s) {
  int len = s.length();
  for (int i = 0 ; i < len ; i++) {
    this->write_((unsigned char)s[i]);
  }
  incCount(len);
}

void outstreamhelp::writeChars(const std::string& s) {
  int len = s.length();
  len = (len % 2 == 0) ? len : len + 1; // 

  for (int i = 0 ; i < len ; i = i+2) {
    this->write_((unsigned char)s[i]);
    this->write_((unsigned char)s[i+1]);
    // int v = s[i];
    // this->write_((v >> 8) & 0xFF);
    // this->write_((v >> 0) & 0xFF);
  }
  incCount(len);
}

void outstreamhelp::writeUTF(const std::string& str) {
  LOG_F(INFO, "outstreamhelp::writeUTF - write len %d, :%s.", str.length(), str.c_str());
  writeUTF(str, *this);
}

int outstreamhelp::writeUTF(const std::string& str, outstreamhelp& os) {
  int strlen = str.length();
  int utflen = 0;
  int c, count = 0;

  /* use charAt instead of copying String to char array */
  for (int i = 0; i < strlen; i++) {
      c = str[i];
      if ((c >= 0x0001) && (c <= 0x007F)) {
          utflen++;
      } else if (c > 0x07FF) {
          utflen += 3;
      } else {
          utflen += 2;
      }
  }

  if (utflen > 65535) {
    throw 165; // encoded string too long: " + utflen + " bytes"
  }

  unsigned char* bytearr = NULL;
  // if (out instanceof DataOutputStream) {
  //     DataOutputStream dos = (DataOutputStream)out;
  //     if(dos.bytearr == null || (dos.bytearr.length < (utflen+2)))
  //         dos.bytearr = new byte[(utflen*2) + 2];
  //     bytearr = dos.bytearr;
  // } else {
    bytearr = new unsigned char[utflen+2];
    // }

  LOG_F(INFO, "outstreamhelp::writeUTF - utflen %d.", utflen);
  bytearr[count++] = (unsigned char) ((utflen >> 8) & 0xFF);
  bytearr[count++] = (unsigned char) ((utflen >> 0) & 0xFF);

  int i=0;
  for (i=0; i<strlen; i++) {
     c = str[i];
     if (!((c >= 0x0001) && (c <= 0x007F))) break;
     bytearr[count++] = (unsigned char) c;
  }

  for (;i < strlen; i++){
    c = str[i];
    if ((c >= 0x0001) && (c <= 0x007F)) {
      bytearr[count++] = (unsigned char) c;

    } else if (c > 0x07FF) {
      bytearr[count++] = (unsigned char) (0xE0 | ((c >> 12) & 0x0F));
      bytearr[count++] = (unsigned char) (0x80 | ((c >>  6) & 0x3F));
      bytearr[count++] = (unsigned char) (0x80 | ((c >>  0) & 0x3F));
    } else {
      bytearr[count++] = (unsigned char) (0xC0 | ((c >>  6) & 0x1F));
      bytearr[count++] = (unsigned char) (0x80 | ((c >>  0) & 0x3F));
    }
  }
  os.write_(bytearr, 0, utflen+2);
  return utflen + 2;
}

void outstreamhelp::incCount(int value) {
  int64 temp = written + value;
  if (temp < 0) { // Integer.MAX_VALUE;
    LOG_F(ERROR, "outstreamhelp::incCount - written %lld, over max_value.", written);
    throw sdkexception(STREAM_MAX_VALUE);
  }
  written = temp;
}


//////////////// instreamhelp ================

int instreamhelp::read_(void* b, int len) {
  LOG_F(INFO, "instreamhelp::read_ - offset %lld.", offset);
  int readlen = this->read(offset, b, len);
  if(readlen != len) {
    LOG_F(ERROR, "outstreamhelp::read_ - void* readden %lld, failed.", offset);
    throw sdkexception(STREAM_READ_FAILED);
  }
  incCount(len);
  return readlen;
}

int instreamhelp::read_(unsigned char* b, int off, int len) {
  LOG_F(INFO, "instreamhelp::read_ - offset %lld, off %d, len %d.", offset, off, len);
  int readlen = this->read(offset, b+off, len);
  if(readlen != len) {
    LOG_F(ERROR, "outstreamhelp::read_ - unsigned char* readden %lld, failed.", offset);
    throw sdkexception(STREAM_READ_FAILED);
  }
  incCount(len);
  return readlen;
}

void instreamhelp::readFully(unsigned char* b, int len) {
  readFully(b, 0, len);
}

void instreamhelp::readFully(unsigned char* b, int off, int len) {
  if (len < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER);
  int n = 0;
  while (n < len) {
    int count = this->read_(b, off + n, len - n);
    if (count < 0)
      throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER);
    n += count;
  }
  // incCount(n);
}

int instreamhelp::skipBytes(int n) {
  // int total = 0;
  // int cur = 0;

  // while ((total<n) && ((cur = (int) in.skip(n-total)) > 0)) {
  //   total += cur;
  // }
  offset += n;
  return offset;
}

bool instreamhelp::readBoolean() {
  int ch;
  this->read_(&ch, 1);
  if (ch < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER); // not support negative number
  return (ch != 0);
}

unsigned char instreamhelp::readByte() {
  unsigned char ch;
  this->read_(&ch, 1);
  // LOG_F(INFO, "instreamhelp::readByte - char %d, len %d.", ch, reallen);
  // incCount(reallen); // add inc offset
  if (ch < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER); // not support negative number
  return (unsigned char)(ch);
}

int instreamhelp::readUnsignedByte() {
  unsigned char ch;
  this->read_(&ch, 1);

  if (ch < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER); // not support negative number
  return ch;
}

short instreamhelp::readShort() {
  int ch1 = this->readUnsignedByte();
  int ch2 = this->readUnsignedByte();
  if ((ch1 | ch2) < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER); // not support negative number
  return (short)((ch1 << 8) + (ch2 << 0));
}

int instreamhelp::readUnsignedShort() {
  int ch1 = this->readUnsignedByte();
  int ch2 = this->readUnsignedByte();
  if ((ch1 | ch2) < 0)
    throw 122; // new EOFException();
  return (ch1 << 8) + (ch2 << 0);
}

char instreamhelp::readChar() {
  int ch1 = this->readUnsignedByte();
  int ch2 = this->readUnsignedByte();
  if ((ch1 | ch2) < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER); // not support negative number
  return (char)((ch1 << 8) + (ch2 << 0));
}

// 支持负数的
int instreamhelp::readInt() {
  unsigned char ch1 = this->readByte();
  unsigned char ch2 = this->readByte();
  unsigned char ch3 = this->readByte();
  unsigned char ch4 = this->readByte();
  if ((ch1 | ch2 | ch3 | ch4) < 0)
    throw sdkexception(STREAM_NOT_SUPPORT_NEGATIVE_NUMBER);
  return ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
}

long instreamhelp::readLong() {
  memset(readBuffer, 0, 8);
  readFully(readBuffer, 0, 8);
  return (((long)readBuffer[0] << 56) +
    ((long)(readBuffer[1] & 255) << 48) +
    ((long)(readBuffer[2] & 255) << 40) +
    ((long)(readBuffer[3] & 255) << 32) +
    ((long)(readBuffer[4] & 255) << 24) +
    ((readBuffer[5] & 255) << 16) +
    ((readBuffer[6] & 255) <<  8) +
    ((readBuffer[7] & 255) <<  0));
}

float instreamhelp::readFloat() {
  LOG_F(ERROR, "outstreamhelp::readFloat - not implement.");
  assert(false);
  return readInt(); //Float.intBitsToFloat(readInt());
}

double instreamhelp::readDouble() {
  LOG_F(ERROR, "outstreamhelp::readDouble - not implement.");
  assert(false);
  return readLong(); //Double.longBitsToDouble(readLong());
}

std::string instreamhelp::readLine() {
  LOG_F(ERROR, "outstreamhelp::readLine - not implement.");
  assert(false);
  return string("line");
}

std::string instreamhelp::readUTF() {
  return readUTF(*this);
}

std::string instreamhelp::readUTF(instreamhelp& in) {
  int utflen = in.readUnsignedShort();
  LOG_F(INFO, "outstreamhelp::readUTF - utflen %d.", utflen);
  unsigned char* bytearr = NULL;
  unsigned char* chararr = NULL;
  // if (in instanceof DataInputStream) {
  //     DataInputStream dis = (DataInputStream)in;
  //     if (dis.bytearr.length < utflen){
  //         dis.bytearr = new char[utflen*2];
  //         dis.chararr = new char[utflen*2];
  //     }
  //     chararr = dis.chararr;
  //     bytearr = dis.bytearr;
  // } else {
    bytearr = new unsigned char[utflen];
    chararr = new unsigned char[utflen];
  // }

  int c, char2, char3;
  int count = 0;
  int chararr_count=0;

  in.readFully(bytearr, 0, utflen);

  while (count < utflen) {
    c = (int) bytearr[count] & 0xff;
    if (c > 127) break;
    count++;
    chararr[chararr_count++]=(unsigned char)c;
  }

  while (count < utflen) {
    c = (int) bytearr[count] & 0xff;
    switch (c >> 4) {
      case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
        /* 0xxxxxxx*/
        count++;
        chararr[chararr_count++]=(unsigned char)c;
        break;
      case 12: case 13:
        /* 110x xxxx   10xx xxxx*/
        count += 2;
        if (count > utflen)
          throw 187; // new UTFDataFormatException(
                // "malformed input: partial character at end");
        char2 = (int) bytearr[count-1];
        if ((char2 & 0xC0) != 0x80)
          throw 188; //new UTFDataFormatException(
                // "malformed input around byte " + count);
        chararr[chararr_count++]=(unsigned char)(((c & 0x1F) << 6) | 
                                          (char2 & 0x3F));
        break;
      case 14:
          /* 1110 xxxx  10xx xxxx  10xx xxxx */
          count += 3;
          if (count > utflen)
            throw 189; //new UTFDataFormatException(
                  // "malformed input: partial character at end");
          char2 = (int) bytearr[count-2];
          char3 = (int) bytearr[count-1];
          if (((char2 & 0xC0) != 0x80) || ((char3 & 0xC0) != 0x80))
            throw 190; //new UTFDataFormatException(
                  // "malformed input around byte " + (count-1));
          chararr[chararr_count++]=(unsigned char)(((c     & 0x0F) << 12) |
                                          ((char2 & 0x3F) << 6)  |
                                          ((char3 & 0x3F) << 0));
          break;
      default:
          /* 10xx xxxx,  1111 xxxx */
          throw 191; //new UTFDataFormatException(
              //"malformed input around byte " + count);
    }
  }
  // The number of chars produced may be less than utflen
  return std::string((char*)chararr).substr(0, chararr_count);
}

} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {
