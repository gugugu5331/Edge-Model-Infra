#pragma once

#include "InetAddress.h"
#include "EventLoop.h"
#include <memory>
#include <functional>
#include <unordered_map>
#include <atomic>

namespace edge_infra {
namespace network {

class TcpConnection;
class Acceptor;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, const std::string&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

class TcpServer {
private:
    EventLoop* loop_;
    const std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    
    ConnectionCallback connection_callback_;
    MessageCallback message_callback_;
    CloseCallback close_callback_;
    
    std::unordered_map<std::string, TcpConnectionPtr> connections_;
    std::atomic<int> next_conn_id_;
    
    bool started_;
    
    // 服务器统计
    std::atomic<uint64_t> total_connections_;
    std::atomic<uint64_t> active_connections_;
    
public:
    TcpServer(EventLoop* loop, const InetAddress& listen_addr, const std::string& name);
    ~TcpServer();
    
    // 禁用拷贝
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;
    
    // 服务器控制
    void start();
    void stop();
    bool isStarted() const { return started_; }
    
    // 回调设置
    void setConnectionCallback(const ConnectionCallback& cb) { connection_callback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { message_callback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { close_callback_ = cb; }
    
    // 连接管理
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
    
    // 广播消息
    void broadcastMessage(const std::string& message);
    void sendToConnection(const std::string& conn_name, const std::string& message);
    
    // 统计信息
    size_t getConnectionCount() const { return connections_.size(); }
    uint64_t getTotalConnections() const { return total_connections_.load(); }
    uint64_t getActiveConnections() const { return active_connections_.load(); }
    
    // 调试功能
    void printConnections() const;
    std::vector<std::string> getConnectionNames() const;
    
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }
    
private:
    void newConnection(int sockfd, const InetAddress& peer_addr);
    std::string generateConnectionName();
};

} // namespace network
} // namespace edge_infra
