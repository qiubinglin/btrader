#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

int main() {
    // 创建一个日志器，输出到指定路径
    auto logger = spdlog::basic_logger_mt("file_logger", "./my_log.log");

    // 设置为默认日志器（可选）
    spdlog::set_default_logger(logger);

    spdlog::info("This goes to the log file!");
}