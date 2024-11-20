#pragma once

#include "StackFlow.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

namespace edge_infra {
namespace infra_controller {

// 通道类型定义
enum class ChannelType {
    POINT_TO_POINT,    // 点对点通信
    PUBLISH_SUBSCRIBE, // 发布订阅
    REQUEST_RESPONSE,  // 请求响应
    BROADCAST,         // 广播
    MULTICAST          // 组播
};

// 消息优先级
enum class MessagePriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    CRITICAL = 3
};

// 通道消息
struct ChannelMessage {
    std::string id;
    std::string sender;
    std::string receiver;
    std::string topic;
    std::string content;
    MessagePriority priority;
    uint64_t timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    ChannelMessage();
    ChannelMessage(const std::string& content, MessagePriority prio = MessagePriority::NORMAL);
    
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    
    std::string toString() const;
};

// 消息过滤器
class MessageFilter {
public:
    virtual ~MessageFilter() = default;
    virtual bool shouldProcess(const ChannelMessage& msg) const = 0;
    virtual std::string getFilterName() const = 0;
};

// 简单的主题过滤器
class TopicFilter : public MessageFilter {
private:
    std::string topic_pattern_;
    
public:
    explicit TopicFilter(const std::string& pattern);
    bool shouldProcess(const ChannelMessage& msg) const override;
    std::string getFilterName() const override;
    
private:
    bool matchPattern(const std::string& topic, const std::string& pattern) const;
};

// 发送者过滤器
class SenderFilter : public MessageFilter {
private:
    std::string sender_id_;
    
public:
    explicit SenderFilter(const std::string& sender);
    bool shouldProcess(const ChannelMessage& msg) const override;
    std::string getFilterName() const override;
};

// 通道接口
class Channel {
public:
    using MessageHandler = std::function<void(const ChannelMessage&)>;
    using ErrorHandler = std::function<void(const std::string&)>;
    
protected:
    std::string name_;
    ChannelType type_;
    bool active_;
    std::atomic<uint64_t> messages_sent_;
    std::atomic<uint64_t> messages_received_;
    std::atomic<uint64_t> errors_count_;
    
    std::vector<std::shared_ptr<MessageFilter>> filters_;
    MessageHandler message_handler_;
    ErrorHandler error_handler_;
    
    mutable std::mutex mutex_;
    
public:
    Channel(const std::string& name, ChannelType type);
    virtual ~Channel() = default;
    
    // 禁用拷贝
    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;
    
    // 生命周期
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isActive() const { return active_; }
    
    // 消息操作
    virtual bool send(const ChannelMessage& msg) = 0;
    virtual bool send(const std::string& content, const std::string& topic = "") = 0;
    
    // 订阅/取消订阅（仅适用于某些通道类型）
    virtual bool subscribe(const std::string& topic) { return false; }
    virtual bool unsubscribe(const std::string& topic) { return false; }
    
    // 过滤器管理
    void addFilter(std::shared_ptr<MessageFilter> filter);
    void removeFilter(const std::string& filter_name);
    void clearFilters();
    
    // 回调设置
    void setMessageHandler(MessageHandler handler) { message_handler_ = handler; }
    void setErrorHandler(ErrorHandler handler) { error_handler_ = handler; }
    
    // 属性访问
    const std::string& getName() const { return name_; }
    ChannelType getType() const { return type_; }
    
    // 统计信息
    uint64_t getMessagesSent() const { return messages_sent_.load(); }
    uint64_t getMessagesReceived() const { return messages_received_.load(); }
    uint64_t getErrorsCount() const { return errors_count_.load(); }
    
    // 调试功能
    virtual void printStatistics() const;
    virtual std::string getStatusString() const;
    
protected:
    bool applyFilters(const ChannelMessage& msg) const;
    void notifyMessageReceived(const ChannelMessage& msg);
    void notifyError(const std::string& error);
    
    void updateSendStats() { messages_sent_++; }
    void updateReceiveStats() { messages_received_++; }
    void updateErrorStats() { errors_count_++; }
};

// ZeroMQ通道实现
class ZmqChannel : public Channel {
private:
    std::string endpoint_;
    void* zmq_socket_;
    std::thread receive_thread_;
    std::atomic<bool> stop_requested_;
    
public:
    ZmqChannel(const std::string& name, ChannelType type, const std::string& endpoint);
    ~ZmqChannel() override;
    
    bool start() override;
    void stop() override;
    
    bool send(const ChannelMessage& msg) override;
    bool send(const std::string& content, const std::string& topic = "") override;
    
    bool subscribe(const std::string& topic) override;
    bool unsubscribe(const std::string& topic) override;
    
private:
    void receiveLoop();
    bool initializeSocket();
    void cleanupSocket();
};

// 通道管理器
class ChannelManager {
private:
    std::unordered_map<std::string, std::shared_ptr<Channel>> channels_;
    mutable std::mutex channels_mutex_;
    
    // 路由表：topic -> channel names
    std::unordered_map<std::string, std::vector<std::string>> routing_table_;
    mutable std::mutex routing_mutex_;
    
public:
    ChannelManager() = default;
    ~ChannelManager();
    
    // 禁用拷贝
    ChannelManager(const ChannelManager&) = delete;
    ChannelManager& operator=(const ChannelManager&) = delete;
    
    // 通道管理
    bool registerChannel(std::shared_ptr<Channel> channel);
    bool unregisterChannel(const std::string& name);
    std::shared_ptr<Channel> getChannel(const std::string& name) const;
    
    // 批量操作
    void startAllChannels();
    void stopAllChannels();
    
    // 路由管理
    void addRoute(const std::string& topic, const std::string& channel_name);
    void removeRoute(const std::string& topic, const std::string& channel_name);
    void clearRoutes(const std::string& topic);
    
    // 消息路由
    bool routeMessage(const ChannelMessage& msg);
    bool routeMessage(const std::string& topic, const std::string& content);
    
    // 广播
    void broadcast(const ChannelMessage& msg);
    void broadcast(const std::string& content);
    
    // 查询功能
    std::vector<std::string> getChannelNames() const;
    std::vector<std::string> getChannelsForTopic(const std::string& topic) const;
    size_t getChannelCount() const;
    
    // 统计和调试
    void printAllStatistics() const;
    void printRoutingTable() const;
};

// 工具函数
std::string channelTypeToString(ChannelType type);
std::string messagePriorityToString(MessagePriority priority);

} // namespace infra_controller
} // namespace edge_infra
