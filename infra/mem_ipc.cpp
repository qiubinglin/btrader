#include "mem_ipc.hpp"

#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>

MemIPC::MemIPC(const std::string &CommId, size_t MemSize, bool Master)
    : m_Id(CommId), m_MemSize(MemSize + sizeof(size_t)), m_RemainSize(MemSize + sizeof(size_t)), m_Master(Master) {
    m_Sem = m_Master ? sem_open(m_Id.c_str(), O_CREAT | O_RDWR, 0666, 0) : sem_open(m_Id.c_str(), O_RDWR);

    if (m_Sem == SEM_FAILED) {
        freeResource();
        throw std::runtime_error("Open Sem failed!!!");
    }

    m_ShmFd = shm_open(m_Id.c_str(), O_CREAT | O_RDWR, 0666);
    if (m_ShmFd == -1) {
        freeResource();
        throw std::runtime_error("Open ShmFd failed!!!");
    }
    if (ftruncate(m_ShmFd, m_MemSize) == -1) {
        freeResource();
        throw std::runtime_error("ftruncate failed!!!");
    }
    void *Buf = mmap(0, m_MemSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_ShmFd, 0);
    if (Buf == MAP_FAILED) {
        freeResource();
        throw std::runtime_error("mmap failed!!!");
    }
    m_SharedMem = (char *)Buf;
    m_DataLen = (size_t *)m_SharedMem;
    close(m_ShmFd);
    m_ShmFd = 0;
}

MemIPC::~MemIPC() { freeResource(); }

MemIPC::Data MemIPC::read() {
    sem_wait(m_Sem);
    MemIPC::Data res;
    res.m_Data = m_SharedMem + usedSize();
    res.m_Len = *m_DataLen;
    return res;
}

char *MemIPC::openData(size_t Len) {
    if (m_OpenDataLen != 0 or m_RemainSize < Len or Len == 0) {
        return nullptr;
    }
    char *Head = m_SharedMem + usedSize();
    // m_RemainSize -= Len; // disable for now
    m_OpenDataLen = Len;
    return Head;
}

void MemIPC::closeData(size_t Len) {
    if (Len != m_OpenDataLen) {
        throw std::runtime_error("openData len must equals to closeData len");
    }
    *m_DataLen = m_OpenDataLen;
    m_OpenDataLen = 0;
    sem_post(m_Sem);
}

void MemIPC::freeResource() {
    if (m_Sem) {
        sem_close(m_Sem);
    }
    if (m_ShmFd) {
        close(m_ShmFd);
    }

    if (m_SharedMem and munmap((void *)m_SharedMem, m_MemSize) == -1) {
        std::cerr << "munmap failed" << std::endl;
    }
}

size_t MemIPC::usedSize() const { return m_MemSize - m_RemainSize + sizeof(size_t); }