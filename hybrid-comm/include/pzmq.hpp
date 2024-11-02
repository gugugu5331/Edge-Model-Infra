#pragma once

#include <zmq.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace edge_infra {
namespace hybrid_comm {

// ZeroMQ消息封装
class ZmqMessage {
private:
    zmq_msg_t msg_;
    bool initialized_;
    
public:
    ZmqMessage();
    explicit ZmqMessage(size_t size);
    explicit ZmqMessage(const std::string& data);
    explicit ZmqMessage(const void* data, size_t size);
    ~ZmqMessage();
    
    // 禁用拷贝，允许移动
    ZmqMessage(const ZmqMessage&) = delete;
    ZmqMessage& operator=(const ZmqMessage&) = delete;
    ZmqMessage(ZmqMessage&& other) noexcept;
    ZmqMessage& operator=(ZmqMessage&& other) noexcept;
    
    // 数据访问
    void* data() { return zmq_msg_data(&msg_); }
    const void* data() const { return zmq_msg_data(const_cast<zmq_msg_t*>(&msg_)); }
    size_t size() const { return zmq_msg_size(const_cast<zmq_msg_t*>(&msg_)); }
    
    // 字符串转换
    std::string toString() const;
    void fromString(const std::string& str);
    
    // 原始消息访问
    zmq_msg_t* raw() { return &msg_; }
    const zmq_msg_t* raw() const { return &msg_; }
    
    bool empty() const { return size() == 0; }
    void clear();
    
private:
    void cleanup();
};

// ZeroMQ上下文管理
class ZmqContext {
private:
    void* context_;
    std::atomic<int> ref_count_;
    static std::mutex instance_mutex_;
    static std::shared_ptr<ZmqContext> instance_;
    
    ZmqContext();
    
public:
    ~ZmqContext();
    
    static std::shared_ptr<ZmqContext> getInstance();
    void* getContext() { return context_; }
    
    // 禁用拷贝
    ZmqContext(const ZmqContext&) = delete;
    ZmqContext& operator=(const ZmqContext&) = delete;
};

// ZeroMQ Socket封装
class ZmqSocket {
public:
    enum SocketType {
        REQ = ZMQ_REQ,
        REP = ZMQ_REP,
        DEALER = ZMQ_DEALER,
        ROUTER = ZMQ_ROUTER,
        PUB = ZMQ_PUB,
        SUB = ZMQ_SUB,
        PUSH = ZMQ_PUSH,
        PULL = ZMQ_PULL,
        PAIR = ZMQ_PAIR
    };
    
private:
    std::shared_ptr<ZmqContext> context_;
    void* socket_;
    SocketType type_;
    std::string endpoint_;
    bool connected_;
    
    // 调试和统计
    std::atomic<uint64_t> messages_sent_;
    std::atomic<uint64_t> messages_received_;
    std::atomic<uint64_t> bytes_sent_;
    std::atomic<uint64_t> bytes_received_;
    
public:
    explicit ZmqSocket(SocketType type);
    ~ZmqSocket();
    
    // 禁用拷贝
    ZmqSocket(const ZmqSocket&) = delete;
    ZmqSocket& operator=(const ZmqSocket&) = delete;
    
    // 连接操作
    bool bind(const std::string& endpoint);
    bool connect(const std::string& endpoint);
    void disconnect();
    void close();
    
    // 消息发送/接收
    bool send(const ZmqMessage& msg, int flags = 0);
    bool send(const std::string& data, int flags = 0);
    bool send(const void* data, size_t size, int flags = 0);
    
    bool recv(ZmqMessage& msg, int flags = 0);
    bool recv(std::string& data, int flags = 0);
    
    // 多部分消息
    bool sendMore(const ZmqMessage& msg);
    bool sendMore(const std::string& data);
    bool hasMore() const;
    
    // Socket选项
    bool setSockOpt(int option, const void* optval, size_t optvallen);
    bool getSockOpt(int option, void* optval, size_t* optvallen);
    
    // 便捷选项设置
    bool setIdentity(const std::string& identity);
    bool setSubscribe(const std::string& filter);
    bool setUnsubscribe(const std::string& filter);
    bool setLinger(int linger_ms);
    bool setReceiveTimeout(int timeout_ms);
    bool setSendTimeout(int timeout_ms);
    
    // 状态查询
    bool isConnected() const { return connected_; }
    SocketType getType() const { return type_; }
    const std::string& getEndpoint() const { return endpoint_; }
    
    // 统计信息
    uint64_t getMessagesSent() const { return messages_sent_.load(); }
    uint64_t getMessagesReceived() const { return messages_received_.load(); }
    uint64_t getBytesSent() const { return bytes_sent_.load(); }
    uint64_t getBytesReceived() const { return bytes_received_.load(); }
    
    // 调试功能
    void enableDebug(bool enable = true);
    void printStatistics() const;
    
private:
    void updateSendStats(size_t bytes);
    void updateRecvStats(size_t bytes);
    
    bool debug_enabled_;
    void debugLog(const std::string& message) const;
};

// ZeroMQ轮询器
class ZmqPoller {
private:
    std::vector<zmq_pollitem_t> poll_items_;
    std::unordered_map<void*, size_t> socket_index_map_;
    
public:
    ZmqPoller() = default;
    ~ZmqPoller() = default;
    
    // 添加/移除socket
    void addSocket(ZmqSocket& socket, short events = ZMQ_POLLIN);
    void removeSocket(ZmqSocket& socket);
    void clear();
    
    // 轮询
    int poll(long timeout_ms = -1);
    
    // 检查事件
    bool hasInput(const ZmqSocket& socket) const;
    bool hasOutput(const ZmqSocket& socket) const;
    bool hasError(const ZmqSocket& socket) const;
    
    size_t size() const { return poll_items_.size(); }
    
private:
    size_t findSocketIndex(void* socket) const;
};

} // namespace hybrid_comm
} // namespace edge_infra
