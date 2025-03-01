/**
 * @file MemIPC.hpp
 * @author qiubinglin (qiubinglin@outlook.com)
 * @brief
 * @version 0.1
 * @date 2025-01-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <semaphore.h>

#include <string>

class MemIPC {
 public:
  struct Data {
    const char *m_Data = nullptr;
    size_t m_Len = 0;
  };

  MemIPC(const std::string &CommId, size_t MemSize, bool Master);
  MemIPC(const MemIPC &That) = delete;
  ~MemIPC();

  Data read();

  char *openData(size_t Len);
  void closeData(size_t Len);

 protected:
  void freeResource();
  size_t usedSize() const;

 protected:
  std::string m_Id;
  size_t m_MemSize = 0;
  size_t m_RemainSize = 0;
  bool m_Master = false;

  char *m_SharedMem = nullptr;
  sem_t *m_Sem = nullptr;
  int m_ShmFd = 0;

  size_t m_OpenDataLen = 0;
  size_t *m_DataLen = nullptr;
};
