#include "BiMemIPC.hpp"

BiMemIPC::BiMemIPC(const std::string &ReadName, size_t ReadSize, const std::string &WriteName, size_t WriteSize)
  : m_ReadChn(ReadName, ReadSize, true),
    m_WriteChn(WriteName, WriteSize, true) {}

MemIPC::Data BiMemIPC::read() {
  return m_ReadChn.read();
}

char *BiMemIPC::openData(size_t Len) {
  return m_WriteChn.openData(Len);
}

void BiMemIPC::finishData(size_t Len) {
  m_WriteChn.closeData(Len);
}

MemIPC::Data BiMemIPC::syncFinishData(size_t Len) {
  m_WriteChn.closeData(Len);
  return read();
}