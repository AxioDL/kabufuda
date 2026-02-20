#include "kabufuda/AsyncIO.hpp"

#include <cstdio>
#include <cstring>

namespace kabufuda {

AsyncIO::AsyncIO(std::string_view filename, bool truncate) {
  m_fd = fopen(filename.data(), truncate ? "wb+" : "rb+");
}

AsyncIO::~AsyncIO() {
  if (*this) {
    fclose(m_fd);
  }
}

AsyncIO::AsyncIO(AsyncIO&& other) {
  m_fd = other.m_fd;
  other.m_fd = nullptr;
  m_maxBlock = other.m_maxBlock;
}

AsyncIO& AsyncIO::operator=(AsyncIO&& other) {
  if (*this) {
    fclose(m_fd);
  }
  m_fd = other.m_fd;
  other.m_fd = nullptr;
  m_maxBlock = other.m_maxBlock;
  return *this;
}

void AsyncIO::_waitForOperation(size_t qIdx) const {}

bool AsyncIO::asyncRead(size_t qIdx, void* buf, size_t length, off_t offset) {
  if (!m_fd || fseeko(m_fd, offset, SEEK_SET) != 0)
    return false;
  return fread(buf, 1, length, m_fd) == length;
}

bool AsyncIO::asyncWrite(size_t qIdx, const void* buf, size_t length, off_t offset) {
  if (!m_fd || fseeko(m_fd, offset, SEEK_SET) != 0)
    return false;
  return fwrite(buf, 1, length, m_fd) == length;
}

ECardResult AsyncIO::pollStatus(size_t qIdx, SizeReturn* szRet) const { return ECardResult::READY; }

ECardResult AsyncIO::pollStatus() const { return ECardResult::READY; }

void AsyncIO::waitForCompletion() const {}

void AsyncIO::resizeQueue(size_t queueSz) {}

} // namespace kabufuda
