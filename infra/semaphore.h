#pragma once

#include <semaphore.h>

namespace infra {

struct Sem {
    sem_t sem;

    /**
     * @brief
     *
     * @param pshared Enable share between processes.
     * @param value Indicate the number of resources.
     * @return int
     */
    int init(int pshared, unsigned int value);

    int wait();

    int post();
    int post(size_t limit);

    int destroy();
};

} // namespace infra