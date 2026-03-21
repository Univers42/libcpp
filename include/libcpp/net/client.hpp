#pragma once
// client.hpp — TCP client using POSIX sockets and Message protocol.
//
// Connects to a server, sends and receives Message objects.

#include "libcpp/net/message.hpp"
#include <functional>
#include <map>
#include <string>
#include <thread>

namespace libcpp {
namespace net {

class Client {
public:
    Client();
    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;
    ~Client();

    // Connect to a remote server
    void connect(const std::string& host, uint16_t port);

    // Disconnect from the server
    void disconnect();

    // Send a message to the server
    void send(const Message& msg);

    // Register a handler for incoming messages of a given type
    void registerHandler(MessageType type, std::function<void(Message&)> handler);

    // Start receiving messages in a background thread
    void startReceiving();

    bool isConnected() const;

private:
    void _receiveLoop();

    // Send raw bytes with a length prefix
    static void _sendRaw(int fd, const std::vector<uint8_t>& data);
    // Receive exactly n bytes
    static bool _recvExact(int fd, uint8_t* buf, size_t n);

    int              _sockFd    = -1;
    bool             _connected = false;
    bool             _receiving = false;
    std::thread      _recvThread;
    MessageConsumer  _consumer;
};

} // namespace net
} // namespace libcpp
