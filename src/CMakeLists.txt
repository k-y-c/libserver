set(
    SRCS
    Channel.cpp
    Epoller.cpp
    EventLoop.cpp
    TcpConnection.cpp
    Thread.cpp
    Timer.cpp
)

add_library(server_net ${SRCS})
target_link_libraries(server_net pthread)