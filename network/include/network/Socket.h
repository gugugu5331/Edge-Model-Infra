#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

namespace edge_infra {
namespace network {

class Socket {
private:
    int sockfd_;
    bool is_connected_;
    
public:
    Socket();
    explicit Socket(int sockfd);
    ~Socket();
    
    // 禁用拷贝构造和赋值
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    
    // 移动构造和赋值
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;
    
    // 基础操作
    bool create();
    bool bind(const std::string& ip, int port);
    bool listen(int backlog = 128);
    Socket accept();
    bool connect(const std::string& ip, int port);
    void close();
    
    // 数据传输
    ssize_t send(const void* data, size_t len);
    ssize_t recv(void* buffer, size_t len);
    ssize_t sendTo(const void* data, size_t len, const sockaddr_in& addr);
    ssize_t recvFrom(void* buffer, size_t len, sockaddr_in& addr);
    
    // Socket选项
    bool setReuseAddr(bool enable = true);
    bool setNonBlocking(bool enable = true);
    bool setKeepAlive(bool enable = true);
    bool setNoDelay(bool enable = true);
    
    // 获取信息
    int getFd() const { return sockfd_; }
    bool isValid() const { return sockfd_ >= 0; }
    bool isConnected() const { return is_connected_; }
    
    std::string getLocalAddress() const;
    int getLocalPort() const;
    std::string getPeerAddress() const;
    int getPeerPort() const;
    
    // 错误处理
    int getLastError() const;
    std::string getErrorString(int error_code) const;
    
private:
    void reset();
    bool setSocketOption(int level, int optname, const void* optval, socklen_t optlen);
};

} // namespace network
} // namespace edge_infra
