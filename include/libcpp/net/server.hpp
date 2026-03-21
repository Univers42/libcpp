#pragma once
// server.hpp — TCP server using POSIX sockets and Message protocol.
//
// Listens for connections, accepts clients, dispatches messages.

#include "libcpp/net/message.hpp"
#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace libcpp {
namespace net {

/// @brief TCP server with per-client threads and message dispatch.
class Server {
public:
    using ClientId = uint64_t;

    Server();
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    ~Server();

    // Start listening on a port (non-blocking — spawns accept thread)
    void start(uint16_t port);

    // Graceful shutdown
    void stop();

    // Send a message to a specific client
    void sendTo(ClientId clientId, const Message& msg);

    // Broadcast to all connected clients
    void sendToAll(const Message& msg);

    // Register handler for incoming message type
    void registerHandler(MessageType type,
                         std::function<void(ClientId, Message&)> handler);

    // Connection / disconnection callbacks
    void onClientConnected(std::function<void(ClientId)> callback);
    void onClientDisconnected(std::function<void(ClientId)> callback);

    bool isRunning() const;

private:
    void _acceptLoop();
    void _clientLoop(ClientId id, int clientFd);

    // Raw send with length prefix
    static void _sendRaw(int fd, const std::vector<uint8_t>& data);
    static bool _recvExact(int fd, uint8_t* buf, size_t n);

    int                       _listenFd = -1;
    std::atomic<bool>         _running{false};
    std::thread               _acceptThread;
    std::atomic<ClientId>     _nextId{1};

    // Active client sockets
    std::mutex                _clientsMtx;
    std::map<ClientId, int>   _clients;        // id -> fd
    std::vector<std::thread>  _clientThreads;

    // Message dispatch
    std::map<MessageType, std::function<void(ClientId, Message&)>> _handlers;
    std::mutex _handlersMtx;

    // Connection callbacks
    std::function<void(ClientId)> _onConnect;
    std::function<void(ClientId)> _onDisconnect;
};

} // namespace net
} // namespace libcpp
