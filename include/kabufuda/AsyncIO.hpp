#pragma once

#ifndef _WIN32
#ifdef __SWITCH__
#include <sys/types.h>
using SizeReturn = ssize_t;
#else
#include <aio.h>
using SizeReturn = ssize_t;
#endif
#else
using SizeReturn = unsigned long;
#endif

#include <cstddef>
#include <vector>

#include "kabufuda/Util.hpp"

namespace kabufuda {
#if _WIN32
struct AsyncIOInner;
#endif

class AsyncIO {
#if defined(__SWITCH__) || defined(EMSCRIPTEN)
  FILE* m_fd;
#elif !defined(_WIN32)
  int m_fd = -1;
  std::vector<std::pair<struct aiocb, SizeReturn>> m_queue;
#else
  AsyncIOInner* m_inner;
#endif
  void _waitForOperation(size_t qIdx) const;
  mutable size_t m_maxBlock = 0;

public:
  AsyncIO() = default;
  explicit AsyncIO(std::string_view filename, bool truncate = false);
  ~AsyncIO();
  AsyncIO(AsyncIO&& other);
  AsyncIO& operator=(AsyncIO&& other);
  AsyncIO(const AsyncIO* other) = delete;
  AsyncIO& operator=(const AsyncIO& other) = delete;
  void resizeQueue(size_t queueSz);
  bool asyncRead(size_t qIdx, void* buf, size_t length, off_t offset);
  bool asyncWrite(size_t qIdx, const void* buf, size_t length, off_t offset);
  ECardResult pollStatus(size_t qIdx, SizeReturn* szRet = nullptr) const;
  ECardResult pollStatus() const;
  void waitForCompletion() const;
#if defined(__SWITCH__) || defined(EMSCRIPTEN)
  explicit operator bool() const { return m_fd != nullptr; }
#elif !defined(_WIN32)
  explicit operator bool() const { return m_fd != -1; }
#else
  explicit operator bool() const;
#endif
};

} // namespace kabufuda
