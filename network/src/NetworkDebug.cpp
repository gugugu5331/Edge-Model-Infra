#include "network/NetworkDebug.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace edge_infra {
namespace network {

// 静态成员初始化
std::atomic<bool> NetworkDebug::debug_enabled_{false};
std::atomic<bool> NetworkDebug::performance_monitoring_{false};
std::mutex NetworkDebug::log_mutex_;
std::ofstream NetworkDebug::debug_log_;
NetworkDebug::NetworkStats NetworkDebug::stats_;

void NetworkDebug::enableDebug(bool enable) {
    debug_enabled_.store(enable);
    if (enable) {
        debugLog("NetworkDebug", "Debug logging enabled");
    }
}

void NetworkDebug::enablePerformanceMonitoring(bool enable) {
    performance_monitoring_.store(enable);
    if (enable) {
        debugLog("NetworkDebug", "Performance monitoring enabled");
    }
}

void NetworkDebug::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (debug_log_.is_open()) {
        debug_log_.close();
    }
    debug_log_.open(filename, std::ios::app);
    if (debug_log_.is_open()) {
        debugLog("NetworkDebug", "Log file set to: " + filename);
    }
}

void NetworkDebug::debugLog(const std::string& component, const std::string& message) {
    if (!debug_enabled_.load()) return;
    
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::string log_line = "[" + getCurrentTime() + "] [DEBUG] [" + component + "] " + message;
    
    std::cout << log_line << std::endl;
    if (debug_log_.is_open()) {
        debug_log_ << log_line << std::endl;
        debug_log_.flush();
    }
}

void NetworkDebug::errorLog(const std::string& component, const std::string& error) {
    recordError();
    
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::string log_line = "[" + getCurrentTime() + "] [ERROR] [" + component + "] " + error;
    
    std::cerr << log_line << std::endl;
    if (debug_log_.is_open()) {
        debug_log_ << log_line << std::endl;
        debug_log_.flush();
    }
}

void NetworkDebug::performanceLog(const std::string& operation, double duration_ms) {
    if (!performance_monitoring_.load()) return;
    
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << duration_ms;
    std::string log_line = "[" + getCurrentTime() + "] [PERF] " + operation + " took " + oss.str() + "ms";
    
    std::cout << log_line << std::endl;
    if (debug_log_.is_open()) {
        debug_log_ << log_line << std::endl;
        debug_log_.flush();
    }
}

void NetworkDebug::printStatistics() {
    std::cout << getStatisticsString() << std::endl;
}

std::string NetworkDebug::getStatisticsString() {
    std::ostringstream oss;
    oss << "=== Network Statistics ===" << std::endl;
    oss << "Bytes Sent: " << formatBytes(getBytesSent()) << std::endl;
    oss << "Bytes Received: " << formatBytes(getBytesReceived()) << std::endl;
    oss << "Connections Created: " << getConnectionsCreated() << std::endl;
    oss << "Connections Closed: " << getConnectionsClosed() << std::endl;
    oss << "Active Connections: " << (getConnectionsCreated() - getConnectionsClosed()) << std::endl;
    oss << "Events Processed: " << getEventsProcessed() << std::endl;
    oss << "Errors Count: " << getErrorsCount() << std::endl;
    return oss.str();
}

void NetworkDebug::resetStatistics() {
    stats_.bytes_sent.store(0);
    stats_.bytes_received.store(0);
    stats_.connections_created.store(0);
    stats_.connections_closed.store(0);
    stats_.events_processed.store(0);
    stats_.errors_count.store(0);
    debugLog("NetworkDebug", "Statistics reset");
}

void NetworkDebug::logConnectionInfo(int fd, const std::string& local_addr, 
                                    const std::string& peer_addr, const std::string& action) {
    if (!debug_enabled_.load()) return;
    
    std::ostringstream oss;
    oss << "Connection [fd=" << fd << "] " << action 
        << " local=" << local_addr << " peer=" << peer_addr;
    debugLog("Connection", oss.str());
}

void NetworkDebug::logPacketInfo(int fd, const std::string& direction, 
                                size_t size, const std::string& data_preview) {
    if (!debug_enabled_.load()) return;
    
    std::ostringstream oss;
    oss << "Packet [fd=" << fd << "] " << direction << " " << size << " bytes";
    if (!data_preview.empty()) {
        oss << " data: " << data_preview.substr(0, 50);
        if (data_preview.length() > 50) oss << "...";
    }
    debugLog("Packet", oss.str());
}

void NetworkDebug::logSocketError(int fd, int error_code, const std::string& operation) {
    std::ostringstream oss;
    oss << "Socket [fd=" << fd << "] " << operation << " failed with error " << error_code;
    errorLog("Socket", oss.str());
}

std::string NetworkDebug::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string NetworkDebug::formatBytes(uint64_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        unit++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}

} // namespace network
} // namespace edge_infra
