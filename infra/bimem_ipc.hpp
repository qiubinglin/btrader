#pragma once

#include "infra/mem_ipc.hpp"

class BiMemIPC {
public:
    BiMemIPC(const std::string &ReadName, size_t ReadSize, const std::string &WriteName, size_t WriteSize);
    BiMemIPC(const BiMemIPC &That) = delete;

    MemIPC::Data read();

    char *openData(size_t Len);
    void finishData(size_t Len);
    MemIPC::Data syncFinishData(size_t Len);

private:
    MemIPC m_ReadChn;
    MemIPC m_WriteChn;
};