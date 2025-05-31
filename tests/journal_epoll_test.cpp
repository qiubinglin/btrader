#include "core/journal/journal.h"

int main() {
    std::vector<btra::journal::JourIndicator> inds(1);
    for (auto &ind : inds) {
        ind.init();
    }
    // std::vector<std::string> names = {"j1", "j2", "j3", "j4", "j5"};

    // Json::json name2fd;
    // for (size_t i = 0; i < inds.size(); ++i) {
    //     name2fd[names[i]] = inds[i].get_fd();
    // }

    for (size_t i = 0; i < inds.size(); ++i) {
        int pid = fork();
        if (pid == 0) {
            inds[i].post();
            while (1) {
                printf("post done\n");
                sleep(3);
            }
        }
    }
    sleep(3);

    int c_id = fork();
    if (c_id == 0) {
        btra::journal::JourObserver observer;
        observer.init();
        for (auto &ind : inds) {
            observer.add_target(ind.get_fd());
        }
        int cnt = 0;
        while (1) {
            cnt++;
            observer.wait();
            printf("handle %d done\n", cnt);
        }
    }

    int c_id2 = fork();
    if (c_id2 == 0) {
        btra::journal::JourObserver observer;
        observer.init();
        for (auto &ind : inds) {
            observer.add_target(ind.get_fd());
        }
        int cnt = 0;
        while (1) {
            cnt++;
            observer.wait();
            // observer.handle();
            printf("##handle %d done\n", cnt);
        }
    }

    int status;
    waitpid(c_id, &status, 0);

    return 0;
}