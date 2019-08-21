
#include "bitsetio.hpp"

namespace sdk {
namespace cores {
namespace utils {



bitsetio::bitsetio(const int64 size, const string& filepath)
  :size_(size), filepath(filepath) {
  if(size <= 0) {
    throw sdkexception(BITSETIO_BITFILE_LENGTH);
  }
  allocated_ = (size >> 3) + 1;
  bits_ = new unsigned char[allocated_];
  if(bits_ == NULL) {
    throw sdkexception(BITSETIO_PTR_NULL);
  }

  if(filepath != "") {
    bitfile = new SyncFile(this->filepath.c_str(), false);
    if(!bitfile || !bitfile->isOpen()) {
      throw sdkexception(BITSETIO_BITFILE_OPEN_FAILED);
    }
  }
}
bitsetio::~bitsetio() {

  if(bits_) { delete []bits_; bits_ = NULL; }
  if(bitfile) { delete bitfile; bitfile = NULL; }
}

bitsetio::bitsetio(const bitsetio& rhs)
:allocated_(rhs.allocated_), size_(rhs.size_) {
  if(allocated_ != 0) {
    bits_ = new unsigned char[allocated_];
    if(bits_ == NULL) {
      throw sdkexception(BITSETIO_PTR_NULL);
    }
    memcpy(bits_, rhs.bits_, (size_ >> 3) + 1);
  }
}

bitsetio& bitsetio::operator=(const bitsetio& rhs) {
  if(this == &rhs) return *this;

  allocated_ = rhs.allocated_;
  size_ = rhs.size_;
  bits_ = new unsigned char[allocated_];
  if(bits_ == NULL) {
    throw sdkexception(BITSETIO_PTR_NULL);
  }
  memcpy(bits_, rhs.bits_, (size_ >> 3) + 1);
  return *this;
}

void bitsetio::set(const int64 position) {
  bits_[position >> 3] |= (1 << (position & 0x7));
}

int64 bitsetio::size() const {
  return size_;
}

bool bitsetio::test(const int64 position) const {
  return (bits_[position >> 3] & (1 << (position & 0x7))) != 0;
}


void bitsetio::reset(const int64 position) {
  bits_[position >> 3] |= (0 << (position & 0x7));
}

int64 bitsetio::count() {
  int64 count = 0;
  for(int i=0; i< size_; ++i) {
    if(test(i))
      ++count;
  }
  return count;
}

bool bitsetio::init_bitfile() {
  
  if(!bitfile || !bitfile->isOpen()) {
    throw sdkexception(BITSETIO_BITFILE_OPEN_FAILED);
  }
  LOG_F(INFO, "bitsetio::init_bitfile - bitset %s.", this->filepath.c_str());

  // 写入初始数据，并大小
  int64 bitfilesize = BITSIZE(size_);
  LOG_F(INFO, "bitsetio::init_bitfile - bitset %s bitfilesize %lld.", this->filepath.c_str(), bitfilesize);
  bitfile->setLength(bitfilesize);

  int64 size = bitfile->getLength();
  if (size != bitfilesize) {
    throw sdkexception(BITSETIO_BITFILE_LENGTH);
  }

  unsigned char buffer[4096] = {0};
  for(int offset = 0; offset < bitfilesize; offset += 4096) {
    bitfile->write(offset, buffer, std::min(4096, (int)(bitfilesize-offset)));
  }

  bitfile->flushFile();
  return true;
}

bool bitsetio::load_bitfile() {

  LOG_F(INFO, "bitsetio::load_bitfile - bitset %s.", this->filepath.c_str());
  if(!bitfile->isOpen()) {
    bitfile->init(this->filepath.c_str(), false);
    if(!bitfile->isOpen()) {
      throw sdkexception(BITSETIO_BITFILE_OPEN_FAILED);
    }
  }

  // 检验文件大小
  int64 size = bitfile->getLength();
  if (size != allocated_) {
    LOG_F(INFO, "bitsetio::load_bitfile - bitset %s, size %lld, allocated %lld.", 
      this->filepath.c_str(), size, allocated_);
    throw sdkexception(BITSETIO_BITFILE_LENGTH);
  }

  // 读取索引文件，并设置bitset
  for(int64 offset = 0; offset < allocated_; offset += 4096) {
    int len = std::min(4096, (int)(allocated_-offset));
    if(bitfile->read(offset, bits_+offset, len) != len) {
      throw sdkexception(BITSETIO_BITFILE_READ_FAILED);
    }
    // LOG_F(INFO, "bitsetio::load_bitfile - bitset read offset %lld.", offset);
  }

  LOG_F(INFO, "bitsetio::load_bitfile - bitset count %lld.", -1); // test this->count());

  return true;
}

bool bitsetio::ext_bitfile(const int64 newsize) {
  LOG_F(INFO, "bitsetio::ext_bitfile - bitset newsize %lld, bitsize %lld.", newsize, BITSIZE(newsize));
  if(!bitfile->isOpen()) {
    bitfile->init(this->filepath.c_str(), false);
    if(!bitfile->isOpen()) {
      throw sdkexception(BITSETIO_BITFILE_OPEN_FAILED);
    }
  }

  int64 bitfilesize = BITSIZE(newsize);
  bitfile->setLength(bitfilesize);
  return true;
}

bool bitsetio::set_bitfile(int64 startindex, int64 endindex, bool value) {
  for(int64 i=startindex; i<endindex; ++i) {
    if(value)
      set(i); // true or false
    else
      reset(i);
  }
  // write to file
  int startoffset = (startindex >> 3);
  int endoffset = (endindex >> 3);
  bitfile->write(startoffset, bits_, endoffset - startoffset);
  return true;
}


} /// namespace utils {
} /// namespace cores {
} /// namespace sdk {
