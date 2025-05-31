#include "infra/semaphore.h"

namespace infra {

int Sem::init(int pshared, unsigned int value) { return sem_init(&sem, pshared, value); }

int Sem::wait() { return sem_wait(&sem); }

int Sem::post() { return sem_post(&sem); }

int Sem::post(size_t limit) {
    int val;
    sem_getvalue(&sem, &val);
    if (val < limit) {
        sem_post(&sem);
    }
}

int Sem::destroy() { return sem_destroy(&sem);}

} // namespace infra