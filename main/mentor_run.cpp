#include <stdlib.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>

#include "core/journal/journal.h"
#include "core/main_cfg.h"
#include "infra/time.h"
#include "mentor.h"

namespace btra {

int Mentor::_run() {
    std::string fds_val;
    std::string fds_data;

#ifndef HP
    std::ifstream f(cfg_file_);
    auto json_cfg = Json::json::parse(f);
    MainCfg cfg_obj(json_cfg);

    auto all_journals = cfg_obj.get_journal_names();
    std::vector<journal::JourIndicator> jour_inds(all_journals.size());
    std::vector<int> eventfd_list;
    for (size_t i = 0; i < all_journals.size(); ++i) {
        jour_inds[i].init();
        fds_val += all_journals[i];
        fds_val += ':';
        fds_val += std::to_string(jour_inds[i].get_fd());
        if (i != all_journals.size() - 1) {
            fds_val += ':';
        }
        eventfd_list.push_back(jour_inds[i].get_fd());
    }
    fds_data = "FDS=" + fds_val;

    if (cfg_obj.run_mode() == enums::RunMode::USER_APP) {
        s_socket_path = "/tmp/btrader-" + infra::time::strfnow(TIMESTAMP_FORMAT2) + ".sock";

        /* Send correct eventfd data. */
        send_eventfds_th_ = std::make_unique<std::thread>([eventfd_list, this]{
            this->send_eventfds(eventfd_list, this->s_socket_path);
        });

        /* The eventfd data is wrong, correct data will is sent by socket */
        std::ofstream ofs(cfg_obj.get_fds_file());
        if (!ofs) {
            throw std::runtime_error("Can not open to write: " + cfg_obj.get_fds_file());
        }
        ofs << fds_val << '\n';
        ofs << s_socket_path; /* line 2: socket_path. */
        ofs.flush();
        ofs.close();
    }

#else
    printf("In HP mode.\n");
#endif

    if (global_state_->is_multi_process) {
        /* Fork sub-processes. */
        /* Fork md. */
        int pid, md_pid, cp_pid, td_pid;
        pid = fork();
        if (pid < 0) {
            throw std::runtime_error("fork failed!");
        } else if (pid == 0) {
            std::string curr_binary = std::filesystem::canonical("/proc/self/exe");
            std::string role_arg = "--role=md";
            std::string cfg_arg = "--cfg=" + cfg_file_;
            char *args[] = {curr_binary.data(), role_arg.data(), cfg_arg.data(), NULL};
            char *envs[] = {fds_data.data(), NULL};
            execvpe(args[0], args, envs);
            exit(EXIT_FAILURE);
        }
        md_pid = pid;

        /* cp */
        pid = fork();
        if (pid < 0) {
            throw std::runtime_error("fork failed!");
        } else if (pid == 0) {
            std::string curr_binary = std::filesystem::canonical("/proc/self/exe");
            std::string role_arg = "--role=cp";
            std::string cfg_arg = "--cfg=" + cfg_file_;
            char *args[] = {curr_binary.data(), role_arg.data(), cfg_arg.data(), NULL};
            char *envs[] = {fds_data.data(), NULL};
            execvpe(args[0], args, envs);
            exit(EXIT_FAILURE);
        }
        cp_pid = pid;

        /* td */
        pid = fork();
        if (pid < 0) {
            throw std::runtime_error("fork failed!");
        } else if (pid == 0) {
            std::string curr_binary = std::filesystem::canonical("/proc/self/exe");
            std::string role_arg = "--role=td";
            std::string cfg_arg = "--cfg=" + cfg_file_;
            char *args[] = {curr_binary.data(), role_arg.data(), cfg_arg.data(), NULL};
            char *envs[] = {fds_data.data(), NULL};
            execvpe(args[0], args, envs);
            exit(EXIT_FAILURE);
        }
        td_pid = pid;

        int status;
        waitpid(md_pid, &status, 0);
        waitpid(cp_pid, &status, 0);
        waitpid(td_pid, &status, 0);
    } else {
        /* Multi-thread */
        setenv("FDS", fds_val.c_str(), 1);

        Mentor md_actor, cp_actor, td_actor;
        md_actor.init("md", cfg_file_, global_state_);
        cp_actor.init("cp", cfg_file_, global_state_);
        td_actor.init("td", cfg_file_, global_state_);

        std::thread md_thread([&md_actor] { md_actor.run(); });
        std::thread cp_thread([&cp_actor] { cp_actor.run(); });
        std::thread td_thread([&td_actor] { td_actor.run(); });

        md_thread.join();
        cp_thread.join();
        td_thread.join();
    }

    return 0;
}

} // namespace btra