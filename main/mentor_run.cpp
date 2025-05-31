#include "mentor.h"

#include "core/journal/journal.h"
#include "core/main_cfg.h"
#include <fstream>

#include "infra/time.h"
#include <cstdlib>
#include <filesystem>
#include <unistd.h>

namespace btra {

int Mentor::_run() {
    std::string fds_json_file;

#ifndef HP
    std::ifstream f(cfg_file_);
    auto json_cfg = Json::json::parse(f);
    MainCfg cfg_obj(json_cfg);

    Json::json name2fd;

    auto all_journals = cfg_obj.get_journal_names();
    std::vector<journal::JourIndicator> jour_inds(all_journals.size());
    for (size_t i = 0; i < all_journals.size(); ++i) {
        jour_inds[i].init();
        name2fd[all_journals[i]] = jour_inds[i].get_fd();
    }

    /* Save name2fd to a temp file. */
    fds_json_file = std::getenv("USER");
    fds_json_file += std::to_string(infra::time::now_in_sec()) + ".json";
    fds_json_file = std::filesystem::current_path().string() + "/" + fds_json_file;
    {
        std::ofstream ofs(fds_json_file);
        ofs << name2fd.dump();
    }
    fds_json_file = "FDS="+ fds_json_file;
#else
    printf("In HP mode.\n");
#endif

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
        char *envs[] = {fds_json_file.data(), NULL};
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
        char *envs[] = {fds_json_file.data(), NULL};
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
        char *envs[] = {fds_json_file.data(), NULL};
        execvpe(args[0], args, envs);
        exit(EXIT_FAILURE);
    }
    td_pid = pid;

    int status;
    waitpid(md_pid, &status, 0);
    waitpid(cp_pid, &status, 0);
    waitpid(td_pid, &status, 0);

    return 0;
}

} // namespace btra