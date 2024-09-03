#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace edge_infra {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {
private:
    LogLevel min_level_;
    std::ofstream file_stream_;
    bool console_output_;
    
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO:  return "INFO ";
            case LogLevel::WARN:  return "WARN ";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
public:
    Logger(LogLevel min_level = LogLevel::INFO, bool console = true) 
        : min_level_(min_level), console_output_(console) {}
    
    ~Logger() {
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
    }
    
    void setLogFile(const std::string& filename) {
        if (file_stream_.is_open()) {
            file_stream_.close();
        }
        file_stream_.open(filename, std::ios::app);
    }
    
    void setMinLevel(LogLevel level) {
        min_level_ = level;
    }
    
    void setConsoleOutput(bool enable) {
        console_output_ = enable;
    }
    
    template<typename... Args>
    void log(LogLevel level, const std::string& format, Args... args) {
        if (level < min_level_) return;
        
        std::string message = formatString(format, args...);
        std::string log_line = "[" + getCurrentTime() + "] [" + 
                              levelToString(level) + "] " + message;
        
        if (console_output_) {
            std::cout << log_line << std::endl;
        }
        
        if (file_stream_.is_open()) {
            file_stream_ << log_line << std::endl;
            file_stream_.flush();
        }
    }
    
    template<typename... Args>
    void debug(const std::string& format, Args... args) {
        log(LogLevel::DEBUG, format, args...);
    }
    
    template<typename... Args>
    void info(const std::string& format, Args... args) {
        log(LogLevel::INFO, format, args...);
    }
    
    template<typename... Args>
    void warn(const std::string& format, Args... args) {
        log(LogLevel::WARN, format, args...);
    }
    
    template<typename... Args>
    void error(const std::string& format, Args... args) {
        log(LogLevel::ERROR, format, args...);
    }
    
private:
    template<typename... Args>
    std::string formatString(const std::string& format, Args... args) {
        // 简单的字符串格式化实现
        std::ostringstream oss;
        formatImpl(oss, format, args...);
        return oss.str();
    }
    
    void formatImpl(std::ostringstream& oss, const std::string& format) {
        oss << format;
    }
    
    template<typename T, typename... Args>
    void formatImpl(std::ostringstream& oss, const std::string& format, T&& t, Args... args) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << t;
            formatImpl(oss, format.substr(pos + 2), args...);
        } else {
            oss << format;
        }
    }
};

// 全局日志实例
extern Logger g_logger;

// 便捷宏定义
#define LOG_DEBUG(...) edge_infra::g_logger.debug(__VA_ARGS__)
#define LOG_INFO(...) edge_infra::g_logger.info(__VA_ARGS__)
#define LOG_WARN(...) edge_infra::g_logger.warn(__VA_ARGS__)
#define LOG_ERROR(...) edge_infra::g_logger.error(__VA_ARGS__)

} // namespace edge_infra
