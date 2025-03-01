#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "infra/MemIPC.hpp"
#include "infra/BiMemIPC.hpp"

class MemIPCTest {
 public:
  void testIPC() {
    std::string CommId = "myipc";
    size_t Memsize = 1024;

    int Pid1 = fork();
    if (Pid1 == 0) {
      MemIPC Ipc(CommId, Memsize, true);
      std::vector<size_t> nums;
      nums.resize(9);
      while (true) {
        char *Head = Ipc.openData(nums.size());
        if (Head == nullptr) {
          std::cout << "openData error" << std::endl;
        }
        memcpy(Head, nums.data(), nums.size());
        nums[0] += 1;
        Ipc.closeData(nums.size());
        sleep(2);
      }
      return;
    }

    int Pid2 = fork();
    if (Pid2 == 0) {
      MemIPC Ipc(CommId, Memsize, true);
      while (true) {
        auto Data = Ipc.read();
        const size_t *nums = (const size_t *)Data.m_Data;
        std::cout << nums[0] << std::endl;
      }
      return;
    }

    std::cout << "parent" << std::endl;
    int status;
    waitpid(Pid1, &status, 0);
    waitpid(Pid2, &status, 0);
  }

  void testBiIPC() {
    std::string CommId1 = "myipc1";
    std::string CommId2 = "myipc2";
    size_t Memsize = 1024;

    int Pid1 = fork();
    if (Pid1 == 0) {
      BiMemIPC Ipc(CommId1, Memsize, CommId2, Memsize);
      std::vector<size_t> nums;
      nums.resize(9);
      while (true) {
        char *Head = Ipc.openData(nums.size());
        if (Head == nullptr) {
          std::cout << "openData error" << std::endl;
        }
        memcpy(Head, nums.data(), nums.size());
        nums[0] += 1;
        auto Data = Ipc.syncFinishData(nums.size());
        const size_t *res = (const size_t *)Data.m_Data;
        std::cout << res[0] << std::endl;
        sleep(2);
      }
      return;
    }

    int Pid2 = fork();
    if (Pid2 == 0) {
      BiMemIPC Ipc(CommId2, Memsize, CommId1, Memsize);
      while (true) {
        auto Data = Ipc.read();
        const size_t *nums = (const size_t *)Data.m_Data;
        std::cout << nums[0] << std::endl;

        char *Head = Ipc.openData(8);
        if (Head == nullptr) {
          std::cout << "openData error" << std::endl;
        }
        memset(Head, 0, 8);
        Ipc.finishData(8);
      }
      return;
    }

    std::cout << "parent" << std::endl;
    int status;
    waitpid(Pid1, &status, 0);
    waitpid(Pid2, &status, 0);
  }
};

int main() {
  MemIPCTest test;
  test.testBiIPC();
  return 0;
}