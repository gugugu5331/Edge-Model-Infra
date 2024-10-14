cmake_minimum_required(VERSION 3.10)
project(EdgeNetworkLayer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# 头文件目录
include_directories(include)
include_directories(../utils)

# 源文件
set(NETWORK_SOURCES
    src/Socket.cpp
    src/InetAddress.cpp
    src/EventLoop.cpp
    src/Channel.cpp
    src/TcpServer.cpp
    src/TcpConnection.cpp
    src/NetworkDebug.cpp
    src/Poller.cpp
    src/Acceptor.cpp
    src/Buffer.cpp
)

# 创建网络层静态库
add_library(edge_network STATIC ${NETWORK_SOURCES})

# 链接库
target_link_libraries(edge_network pthread)

# 安装
install(TARGETS edge_network DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# 测试程序
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# 示例程序
if(BUILD_EXAMPLES)
    add_executable(echo_server examples/echo_server.cpp)
    target_link_libraries(echo_server edge_network)
    
    add_executable(echo_client examples/echo_client.cpp)
    target_link_libraries(echo_client edge_network)
endif()
