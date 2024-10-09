#pragma once

#include "InetAddress.h"
#include "Socket.h"
#include <memory>
#include <functional>
#include <string>
#include <atomic>

namespace edge_infra {
namespace network {

class EventLoop;
class Channel;

using TcpConnectionPtr = std::shared_ptr<class TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, const std::string&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    enum State {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };
    
private:
    EventLoop* loop_;
    const std::string name_;
    std::atomic<State> state_;
    
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;
    
    InetAddress local_addr_;
    InetAddress peer_addr_;
    
    ConnectionCallback connection_callback_;
    MessageCallback message_callback_;
    CloseCallback close_callback_;
    
    std::string input_buffer_;
    std::string output_buffer_;
    
    // 连接统计
    std::atomic<uint64_t> bytes_sent_;
    std::atomic<uint64_t> bytes_received_;
    std::chrono::steady_clock::time_point connect_time_;
    
public:
    TcpConnection(EventLoop* loop, const std::string& name, int sockfd,
                  const InetAddress& local_addr, const InetAddress& peer_addr);
    ~TcpConnection();
    
    // 禁用拷贝
    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator=(const TcpConnection&) = delete;
    
    // 连接控制
    void connectEstablished();
    void connectDestroyed();
    void shutdown();
    void forceClose();
    
    // 数据发送
    void send(const std::string& message);
    void send(const void* data, size_t len);
    
    // 回调设置
    void setConnectionCallback(const ConnectionCallback& cb) { connection_callback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { message_callback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { close_callback_ = cb; }
    
    // 状态查询
    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }
    State getState() const { return state_.load(); }
    
    // 地址信息
    const InetAddress& localAddress() const { return local_addr_; }
    const InetAddress& peerAddress() const { return peer_addr_; }
    std::string localAddressString() const { return local_addr_.toString(); }
    std::string peerAddressString() const { return peer_addr_.toString(); }
    
    // 统计信息
    uint64_t getBytesSent() const { return bytes_sent_.load(); }
    uint64_t getBytesReceived() const { return bytes_received_.load(); }
    std::chrono::steady_clock::duration getConnectDuration() const;
    
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }
    
    // 调试功能
    std::string stateToString() const;
    void enableTcpNoDelay(bool on = true);
    void enableKeepAlive(bool on = true);
    
private:
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();
    
    void sendInLoop(const std::string& message);
    void sendInLoop(const void* data, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    
    void setState(State s) { state_ = s; }
    
    std::string stateToString(State state) const;
};

} // namespace network
} // namespace edge_infra
