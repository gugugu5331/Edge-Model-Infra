#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <sys/epoll.h>

namespace edge_infra {
namespace network {

class Channel;
class Poller;

using EventCallback = std::function<void()>;
using TimerCallback = std::function<void()>;

class EventLoop {
private:
    std::atomic<bool> running_;
    std::atomic<bool> quit_;
    std::thread::id thread_id_;
    
    std::unique_ptr<Poller> poller_;
    std::vector<std::function<void()>> pending_functors_;
    std::mutex functors_mutex_;
    
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;
    
    // 调试和统计信息
    std::atomic<uint64_t> loop_count_;
    std::atomic<uint64_t> event_count_;
    
public:
    EventLoop();
    ~EventLoop();
    
    // 禁用拷贝
    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;
    
    // 主循环
    void loop();
    void quit();
    
    // 在事件循环中执行函数
    void runInLoop(std::function<void()> cb);
    void queueInLoop(std::function<void()> cb);
    
    // Channel管理
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);
    
    // 定时器支持
    void runAfter(double delay, TimerCallback cb);
    void runEvery(double interval, TimerCallback cb);
    
    // 线程安全检查
    bool isInLoopThread() const;
    void assertInLoopThread() const;
    
    // 统计信息
    uint64_t getLoopCount() const { return loop_count_.load(); }
    uint64_t getEventCount() const { return event_count_.load(); }
    
    // 调试功能
    void enableDebug(bool enable = true);
    void printStatistics() const;
    
private:
    void wakeup();
    void handleWakeup();
    void doPendingFunctors();
    
    bool debug_enabled_;
    void debugLog(const std::string& message) const;
};

} // namespace network
} // namespace edge_infra
