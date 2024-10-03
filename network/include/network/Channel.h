#pragma once

#include <functional>
#include <memory>
#include <sys/epoll.h>

namespace edge_infra {
namespace network {

class EventLoop;

class Channel {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void()>;
    
private:
    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_; // 用于Poller
    
    bool event_handling_;
    bool added_to_loop_;
    
    ReadEventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback close_callback_;
    EventCallback error_callback_;
    
    // 调试信息
    std::string debug_name_;
    
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();
    
    // 禁用拷贝
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    
    // 事件处理
    void handleEvent();
    void setReadCallback(ReadEventCallback cb) { read_callback_ = std::move(cb); }
    void setWriteCallback(EventCallback cb) { write_callback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { close_callback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { error_callback_ = std::move(cb); }
    
    // 事件启用/禁用
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    
    // 状态查询
    bool isReading() const { return events_ & kReadEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    
    // Poller使用的接口
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }
    int index() const { return index_; }
    void set_index(int idx) { index_ = idx; }
    
    // 生命周期管理
    void remove();
    EventLoop* ownerLoop() { return loop_; }
    
    // 调试功能
    void setDebugName(const std::string& name) { debug_name_ = name; }
    std::string reventsToString() const;
    std::string eventsToString() const;
    
private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    
    void update();
    void handleEventWithGuard();
    
    std::string eventsToString(int fd, int ev) const;
};

} // namespace network
} // namespace edge_infra
