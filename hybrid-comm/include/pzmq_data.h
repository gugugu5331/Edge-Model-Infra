#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <cstdint>

namespace edge_infra {
namespace hybrid_comm {

// 消息类型定义
enum class MessageType : uint32_t {
    UNKNOWN = 0,
    REQUEST = 1,
    RESPONSE = 2,
    NOTIFICATION = 3,
    HEARTBEAT = 4,
    ERROR = 5,
    DATA_TRANSFER = 6,
    CONTROL = 7
};

// 消息优先级
enum class MessagePriority : uint8_t {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    CRITICAL = 3
};

// 消息头结构
struct MessageHeader {
    uint32_t magic;           // 魔数，用于验证
    uint32_t version;         // 协议版本
    MessageType type;         // 消息类型
    MessagePriority priority; // 消息优先级
    uint32_t sequence_id;     // 序列号
    uint64_t timestamp;       // 时间戳
    uint32_t payload_size;    // 负载大小
    uint32_t checksum;        // 校验和
    char sender_id[32];       // 发送者ID
    char receiver_id[32];     // 接收者ID
    uint32_t flags;           // 标志位
    uint32_t reserved[3];     // 保留字段
    
    MessageHeader();
    void setTimestamp();
    bool isValid() const;
    uint32_t calculateChecksum(const void* payload) const;
};

// 序列化数据包
class SerializedData {
private:
    std::vector<uint8_t> buffer_;
    size_t read_pos_;
    
public:
    SerializedData();
    explicit SerializedData(size_t reserve_size);
    explicit SerializedData(const std::vector<uint8_t>& data);
    explicit SerializedData(const void* data, size_t size);
    
    // 写入操作
    void writeUInt8(uint8_t value);
    void writeUInt16(uint16_t value);
    void writeUInt32(uint32_t value);
    void writeUInt64(uint64_t value);
    void writeInt8(int8_t value);
    void writeInt16(int16_t value);
    void writeInt32(int32_t value);
    void writeInt64(int64_t value);
    void writeFloat(float value);
    void writeDouble(double value);
    void writeString(const std::string& value);
    void writeBytes(const void* data, size_t size);
    void writeBool(bool value);
    
    // 读取操作
    uint8_t readUInt8();
    uint16_t readUInt16();
    uint32_t readUInt32();
    uint64_t readUInt64();
    int8_t readInt8();
    int16_t readInt16();
    int32_t readInt32();
    int64_t readInt64();
    float readFloat();
    double readDouble();
    std::string readString();
    std::vector<uint8_t> readBytes(size_t size);
    bool readBool();
    
    // 数据访问
    const uint8_t* data() const { return buffer_.data(); }
    uint8_t* data() { return buffer_.data(); }
    size_t size() const { return buffer_.size(); }
    size_t capacity() const { return buffer_.capacity(); }
    
    // 位置控制
    size_t getReadPos() const { return read_pos_; }
    void setReadPos(size_t pos) { read_pos_ = pos; }
    void resetReadPos() { read_pos_ = 0; }
    bool hasMoreData() const { return read_pos_ < buffer_.size(); }
    size_t remainingBytes() const { return buffer_.size() - read_pos_; }
    
    // 缓冲区操作
    void clear();
    void reserve(size_t size);
    void resize(size_t size);
    
    // 序列化辅助
    template<typename T>
    void serialize(const T& obj);
    
    template<typename T>
    T deserialize();
    
private:
    void ensureSpace(size_t needed);
    void checkReadBounds(size_t needed) const;
};

// 消息包装器
class Message {
private:
    MessageHeader header_;
    SerializedData payload_;
    
public:
    Message();
    explicit Message(MessageType type);
    Message(MessageType type, const std::string& data);
    
    // 头部操作
    MessageHeader& getHeader() { return header_; }
    const MessageHeader& getHeader() const { return header_; }
    
    void setType(MessageType type) { header_.type = type; }
    MessageType getType() const { return header_.type; }
    
    void setPriority(MessagePriority priority) { header_.priority = priority; }
    MessagePriority getPriority() const { return header_.priority; }
    
    void setSequenceId(uint32_t id) { header_.sequence_id = id; }
    uint32_t getSequenceId() const { return header_.sequence_id; }
    
    void setSenderId(const std::string& id);
    std::string getSenderId() const;
    
    void setReceiverId(const std::string& id);
    std::string getReceiverId() const;
    
    // 负载操作
    SerializedData& getPayload() { return payload_; }
    const SerializedData& getPayload() const { return payload_; }
    
    void setPayload(const SerializedData& data) { payload_ = data; }
    void setPayload(const std::string& data);
    void setPayload(const void* data, size_t size);
    
    // 序列化/反序列化
    std::vector<uint8_t> serialize() const;
    bool deserialize(const std::vector<uint8_t>& data);
    bool deserialize(const void* data, size_t size);
    
    // 验证
    bool validate() const;
    void updateChecksum();
    
    // 工具函数
    size_t getTotalSize() const;
    std::string toString() const;
    
    // 便捷创建方法
    static Message createRequest(const std::string& data);
    static Message createResponse(const std::string& data);
    static Message createNotification(const std::string& data);
    static Message createHeartbeat();
    static Message createError(const std::string& error_msg);
};

// 消息调试工具
class MessageDebugger {
public:
    static void printMessage(const Message& msg);
    static void printHeader(const MessageHeader& header);
    static void printPayload(const SerializedData& payload, size_t max_bytes = 64);
    static std::string messageTypeToString(MessageType type);
    static std::string priorityToString(MessagePriority priority);
    static void enableHexDump(bool enable = true);
    
private:
    static bool hex_dump_enabled_;
    static std::string bytesToHex(const void* data, size_t size);
};

} // namespace hybrid_comm
} // namespace edge_infra
