#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <fstream>

namespace edge_infra {
namespace network {

// 网络调试和监控工具
class NetworkDebug {
private:
    static std::atomic<bool> debug_enabled_;
    static std::atomic<bool> performance_monitoring_;
    static std::mutex log_mutex_;
    static std::ofstream debug_log_;
    
    // 性能统计
    struct NetworkStats {
        std::atomic<uint64_t> bytes_sent{0};
        std::atomic<uint64_t> bytes_received{0};
        std::atomic<uint64_t> connections_created{0};
        std::atomic<uint64_t> connections_closed{0};
        std::atomic<uint64_t> events_processed{0};
        std::atomic<uint64_t> errors_count{0};
    };
    
    static NetworkStats stats_;
    
public:
    // 调试控制
    static void enableDebug(bool enable = true);
    static void enablePerformanceMonitoring(bool enable = true);
    static void setLogFile(const std::string& filename);
    
    // 调试日志
    static void debugLog(const std::string& component, const std::string& message);
    static void errorLog(const std::string& component, const std::string& error);
    static void performanceLog(const std::string& operation, double duration_ms);
    
    // 统计更新
    static void recordBytesSent(size_t bytes) { stats_.bytes_sent += bytes; }
    static void recordBytesReceived(size_t bytes) { stats_.bytes_received += bytes; }
    static void recordConnectionCreated() { stats_.connections_created++; }
    static void recordConnectionClosed() { stats_.connections_closed++; }
    static void recordEventProcessed() { stats_.events_processed++; }
    static void recordError() { stats_.errors_count++; }
    
    // 统计查询
    static uint64_t getBytesSent() { return stats_.bytes_sent.load(); }
    static uint64_t getBytesReceived() { return stats_.bytes_received.load(); }
    static uint64_t getConnectionsCreated() { return stats_.connections_created.load(); }
    static uint64_t getConnectionsClosed() { return stats_.connections_closed.load(); }
    static uint64_t getEventsProcessed() { return stats_.events_processed.load(); }
    static uint64_t getErrorsCount() { return stats_.errors_count.load(); }
    
    // 统计报告
    static void printStatistics();
    static std::string getStatisticsString();
    static void resetStatistics();
    
    // 连接监控
    static void logConnectionInfo(int fd, const std::string& local_addr, 
                                 const std::string& peer_addr, const std::string& action);
    
    // 数据包监控
    static void logPacketInfo(int fd, const std::string& direction, 
                             size_t size, const std::string& data_preview = "");
    
    // 错误监控
    static void logSocketError(int fd, int error_code, const std::string& operation);
    
private:
    static std::string getCurrentTime();
    static std::string formatBytes(uint64_t bytes);
};

// 性能计时器 - 用于测量网络操作耗时
class NetworkTimer {
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::string operation_name_;
    
public:
    explicit NetworkTimer(const std::string& operation) 
        : operation_name_(operation) {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
    ~NetworkTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time_);
        double duration_ms = duration.count() / 1000.0;
        NetworkDebug::performanceLog(operation_name_, duration_ms);
    }
};

// 便捷宏定义
#define NETWORK_DEBUG_LOG(component, message) \
    NetworkDebug::debugLog(component, message)

#define NETWORK_ERROR_LOG(component, error) \
    NetworkDebug::errorLog(component, error)

#define NETWORK_TIMER(operation) \
    NetworkTimer timer(operation)

#define NETWORK_LOG_CONNECTION(fd, local, peer, action) \
    NetworkDebug::logConnectionInfo(fd, local, peer, action)

#define NETWORK_LOG_PACKET(fd, direction, size, preview) \
    NetworkDebug::logPacketInfo(fd, direction, size, preview)

#define NETWORK_LOG_ERROR(fd, error, operation) \
    NetworkDebug::logSocketError(fd, error, operation)

} // namespace network
} // namespace edge_infra
