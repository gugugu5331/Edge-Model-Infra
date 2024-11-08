#include "../include/pzmq_data.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>

namespace edge_infra {
namespace hybrid_comm {

// MessageDebugger静态成员初始化
bool MessageDebugger::hex_dump_enabled_ = false;

void MessageDebugger::printMessage(const Message& msg) {
    std::cout << "=== Message Debug Info ===" << std::endl;
    printHeader(msg.getHeader());
    std::cout << "Payload Size: " << msg.getPayload().size() << " bytes" << std::endl;
    printPayload(msg.getPayload());
    std::cout << "=========================" << std::endl;
}

void MessageDebugger::printHeader(const MessageHeader& header) {
    std::cout << "Header Info:" << std::endl;
    std::cout << "  Magic: 0x" << std::hex << header.magic << std::dec << std::endl;
    std::cout << "  Version: " << header.version << std::endl;
    std::cout << "  Type: " << messageTypeToString(header.type) << std::endl;
    std::cout << "  Priority: " << priorityToString(header.priority) << std::endl;
    std::cout << "  Sequence ID: " << header.sequence_id << std::endl;
    std::cout << "  Timestamp: " << header.timestamp << std::endl;
    std::cout << "  Payload Size: " << header.payload_size << std::endl;
    std::cout << "  Checksum: 0x" << std::hex << header.checksum << std::dec << std::endl;
    std::cout << "  Sender ID: " << std::string(header.sender_id, 
        strnlen(header.sender_id, sizeof(header.sender_id))) << std::endl;
    std::cout << "  Receiver ID: " << std::string(header.receiver_id, 
        strnlen(header.receiver_id, sizeof(header.receiver_id))) << std::endl;
    std::cout << "  Flags: 0x" << std::hex << header.flags << std::dec << std::endl;
}

void MessageDebugger::printPayload(const SerializedData& payload, size_t max_bytes) {
    std::cout << "Payload Info:" << std::endl;
    
    if (payload.size() == 0) {
        std::cout << "  (empty payload)" << std::endl;
        return;
    }
    
    size_t bytes_to_show = std::min(payload.size(), max_bytes);
    std::cout << "  Data (" << bytes_to_show << "/" << payload.size() << " bytes): ";
    
    if (hex_dump_enabled_) {
        std::cout << std::endl;
        std::cout << bytesToHex(payload.data(), bytes_to_show) << std::endl;
    } else {
        // 尝试作为字符串显示
        bool is_printable = true;
        for (size_t i = 0; i < bytes_to_show; ++i) {
            uint8_t byte = payload.data()[i];
            if (byte < 32 || byte > 126) {
                is_printable = false;
                break;
            }
        }
        
        if (is_printable) {
            std::cout << "\"" << std::string(reinterpret_cast<const char*>(payload.data()), bytes_to_show) << "\"";
            if (bytes_to_show < payload.size()) {
                std::cout << "...";
            }
        } else {
            std::cout << bytesToHex(payload.data(), bytes_to_show);
            if (bytes_to_show < payload.size()) {
                std::cout << "...";
            }
        }
        std::cout << std::endl;
    }
}

std::string MessageDebugger::messageTypeToString(MessageType type) {
    switch (type) {
        case MessageType::UNKNOWN: return "UNKNOWN";
        case MessageType::REQUEST: return "REQUEST";
        case MessageType::RESPONSE: return "RESPONSE";
        case MessageType::NOTIFICATION: return "NOTIFICATION";
        case MessageType::HEARTBEAT: return "HEARTBEAT";
        case MessageType::ERROR: return "ERROR";
        case MessageType::DATA_TRANSFER: return "DATA_TRANSFER";
        case MessageType::CONTROL: return "CONTROL";
        default: return "INVALID(" + std::to_string(static_cast<uint32_t>(type)) + ")";
    }
}

std::string MessageDebugger::priorityToString(MessagePriority priority) {
    switch (priority) {
        case MessagePriority::LOW: return "LOW";
        case MessagePriority::NORMAL: return "NORMAL";
        case MessagePriority::HIGH: return "HIGH";
        case MessagePriority::CRITICAL: return "CRITICAL";
        default: return "INVALID(" + std::to_string(static_cast<uint8_t>(priority)) + ")";
    }
}

void MessageDebugger::enableHexDump(bool enable) {
    hex_dump_enabled_ = enable;
}

std::string MessageDebugger::bytesToHex(const void* data, size_t size) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    std::ostringstream oss;
    
    for (size_t i = 0; i < size; ++i) {
        if (i > 0 && i % 16 == 0) {
            oss << std::endl << "  ";
        } else if (i > 0 && i % 8 == 0) {
            oss << "  ";
        } else if (i > 0) {
            oss << " ";
        } else {
            oss << "  ";
        }
        
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    
    return oss.str();
}

// ZMQ消息调试工具
class ZmqMessageDebugger {
public:
    static void logMessageSent(const std::string& socket_type, const std::string& endpoint, 
                              const Message& msg) {
        std::cout << "[ZMQ-SEND] " << socket_type << " -> " << endpoint 
                  << " | Type: " << MessageDebugger::messageTypeToString(msg.getType())
                  << " | Size: " << msg.getTotalSize() << " bytes"
                  << " | SeqID: " << msg.getSequenceId() << std::endl;
    }
    
    static void logMessageReceived(const std::string& socket_type, const std::string& endpoint,
                                  const Message& msg) {
        std::cout << "[ZMQ-RECV] " << socket_type << " <- " << endpoint
                  << " | Type: " << MessageDebugger::messageTypeToString(msg.getType())
                  << " | Size: " << msg.getTotalSize() << " bytes"
                  << " | SeqID: " << msg.getSequenceId() << std::endl;
    }
    
    static void logConnectionEvent(const std::string& socket_type, const std::string& endpoint,
                                  const std::string& event) {
        std::cout << "[ZMQ-CONN] " << socket_type << " | " << endpoint 
                  << " | " << event << std::endl;
    }
    
    static void logError(const std::string& socket_type, const std::string& operation,
                        int error_code, const std::string& error_msg) {
        std::cout << "[ZMQ-ERROR] " << socket_type << " | " << operation
                  << " | Code: " << error_code << " | " << error_msg << std::endl;
    }
};

} // namespace hybrid_comm
} // namespace edge_infra
