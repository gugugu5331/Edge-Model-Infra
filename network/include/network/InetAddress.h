#pragma once

#include <netinet/in.h>
#include <string>

namespace edge_infra {
namespace network {

class InetAddress {
private:
    sockaddr_in addr_;
    
public:
    // 构造函数
    InetAddress();
    InetAddress(const std::string& ip, int port);
    InetAddress(int port); // 绑定到所有接口
    explicit InetAddress(const sockaddr_in& addr);
    
    // 地址操作
    void setAddress(const std::string& ip, int port);
    void setPort(int port);
    void setIP(const std::string& ip);
    
    // 获取信息
    std::string getIP() const;
    int getPort() const;
    std::string toString() const;
    
    // 获取原始地址结构
    const sockaddr_in& getSockAddr() const { return addr_; }
    sockaddr_in& getSockAddr() { return addr_; }
    const sockaddr* getSockAddrPtr() const { 
        return reinterpret_cast<const sockaddr*>(&addr_); 
    }
    sockaddr* getSockAddrPtr() { 
        return reinterpret_cast<sockaddr*>(&addr_); 
    }
    socklen_t getSockAddrLen() const { return sizeof(addr_); }
    
    // 比较操作
    bool operator==(const InetAddress& other) const;
    bool operator!=(const InetAddress& other) const;
    bool operator<(const InetAddress& other) const;
    
    // 静态工具函数
    static InetAddress getLocalAddress();
    static std::vector<InetAddress> getAllLocalAddresses();
    static bool isValidIP(const std::string& ip);
    static std::string hostToIP(const std::string& hostname);
    
private:
    void initAddress();
};

} // namespace network
} // namespace edge_infra
