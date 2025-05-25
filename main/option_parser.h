/**
 * @file option_parser.h
 * @brief from riscv-isa-sim
 *
 */
#pragma once

#include <functional>
#include <vector>

class OptionParser {
public:
    OptionParser() : helpmsg(0) {}
    void help(void (*helpm)(void)) { helpmsg = helpm; }
    void option(char c, const char *s, int arg, std::function<void(const char *)> action);
    const char *const *parse(const char *const *argv0);

private:
    struct option_t {
        char chr;
        const char *str;
        int arg;
        std::function<void(const char *)> func;
        option_t(char chr, const char *str, int arg, std::function<void(const char *)> func)
            : chr(chr), str(str), arg(arg), func(func) {}
    };
    std::vector<option_t> opts;
    void (*helpmsg)(void);
    void error(const char *msg, const char *argv0, const char *arg);
};
