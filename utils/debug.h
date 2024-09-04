#pragma once

#include <iostream>
#include <string>
#include <cassert>

namespace edge_infra {

// 调试宏定义
#ifdef DEBUG
    #define DEBUG_ENABLED 1
#else
    #define DEBUG_ENABLED 0
#endif

// 断言宏
#define EDGE_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << std::endl; \
            std::cerr << "Message: " << message << std::endl; \
            std::cerr << "File: " << __FILE__ << ", Line: " << __LINE__ << std::endl; \
            std::abort(); \
        } \
    } while(0)

// 调试输出宏
#define DEBUG_PRINT(message) \
    do { \
        if (DEBUG_ENABLED) { \
            std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " - " << message << std::endl; \
        } \
    } while(0)

// 错误处理宏
#define EDGE_ERROR(message) \
    do { \
        std::cerr << "[ERROR] " << __FILE__ << ":" << __LINE__ << " - " << message << std::endl; \
    } while(0)

// 警告宏
#define EDGE_WARN(message) \
    do { \
        std::cout << "[WARN] " << __FILE__ << ":" << __LINE__ << " - " << message << std::endl; \
    } while(0)

// 性能计时器
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::string name_;
    
public:
    Timer(const std::string& name) : name_(name) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    ~Timer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        std::cout << "[TIMER] " << name_ << " took " << duration.count() << " microseconds" << std::endl;
    }
};

// 计时宏
#define EDGE_TIMER(name) edge_infra::Timer timer(name)

// 内存使用监控
class MemoryMonitor {
public:
    static void printMemoryUsage(const std::string& tag = "") {
        // 简单的内存使用信息打印
        std::cout << "[MEMORY] " << tag << " - Memory usage check" << std::endl;
        // 在实际实现中，这里会读取 /proc/self/status 或使用其他系统调用
    }
};

// 错误码定义
enum class ErrorCode {
    SUCCESS = 0,
    INVALID_PARAMETER = 1,
    MEMORY_ALLOCATION_FAILED = 2,
    FILE_NOT_FOUND = 3,
    NETWORK_ERROR = 4,
    TIMEOUT = 5,
    UNKNOWN_ERROR = 999
};

// 错误码转字符串
inline std::string errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS: return "Success";
        case ErrorCode::INVALID_PARAMETER: return "Invalid parameter";
        case ErrorCode::MEMORY_ALLOCATION_FAILED: return "Memory allocation failed";
        case ErrorCode::FILE_NOT_FOUND: return "File not found";
        case ErrorCode::NETWORK_ERROR: return "Network error";
        case ErrorCode::TIMEOUT: return "Timeout";
        case ErrorCode::UNKNOWN_ERROR: return "Unknown error";
        default: return "Undefined error";
    }
}

} // namespace edge_infra
