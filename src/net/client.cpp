// client.cpp — TCP Client implementation using POSIX sockets.

#include "libcpp/net/client.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace libcpp {
namespace net {

Client::Client() = default;

Client::~Client() { disconnect(); }

void Client::connect(const std::string& host, uint16_t port)
{
    if (_connected)
        throw std::runtime_error("Client: already connected");

    _sockFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_sockFd < 0)
        throw std::runtime_error("Client: socket() failed");

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    // Try numeric first, then resolve hostname
    if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        struct hostent* he = ::gethostbyname(host.c_str());
        if (!he) {
            ::close(_sockFd);
            _sockFd = -1;
            throw std::runtime_error("Client: cannot resolve host");
        }
        std::memcpy(&addr.sin_addr, he->h_addr_list[0],
                     static_cast<size_t>(he->h_length));
    }

    if (::connect(_sockFd, reinterpret_cast<struct sockaddr*>(&addr),
                  sizeof(addr)) < 0) {
        ::close(_sockFd);
        _sockFd = -1;
        throw std::runtime_error("Client: connect() failed");
    }
    _connected = true;
}

void Client::disconnect()
{
    _receiving = false;
    if (_sockFd >= 0) {
        ::shutdown(_sockFd, SHUT_RDWR);
        ::close(_sockFd);
        _sockFd = -1;
    }
    _connected = false;
    if (_recvThread.joinable())
        _recvThread.join();
}

void Client::send(const Message& msg)
{
    if (!_connected)
        throw std::runtime_error("Client: not connected");
    _sendRaw(_sockFd, msg.serialize());
}

void Client::registerHandler(MessageType type,
                              std::function<void(Message&)> handler)
{
    _consumer.registerHandler(type, std::move(handler));
}

void Client::startReceiving()
{
    if (!_connected)
        throw std::runtime_error("Client: not connected");
    _receiving  = true;
    _recvThread = std::thread(&Client::_receiveLoop, this);
}

bool Client::isConnected() const { return _connected; }

// ── Private helpers ───────────────────────────────────────────────────────

void Client::_receiveLoop()
{
    while (_receiving && _connected) {
        // Read total message size (uint32_t length prefix)
        uint32_t totalLen = 0;
        if (!_recvExact(_sockFd, reinterpret_cast<uint8_t*>(&totalLen),
                        sizeof(totalLen)))
            break;

        std::vector<uint8_t> buf(totalLen);
        if (!_recvExact(_sockFd, buf.data(), totalLen))
            break;

        try {
            Message msg = Message::deserialize(buf);
            _consumer.consume(msg);
        } catch (...) {
            // Silently drop malformed messages
        }
    }
}

void Client::_sendRaw(int fd, const std::vector<uint8_t>& data)
{
    uint32_t len = static_cast<uint32_t>(data.size());
    // Send length prefix
    if (::send(fd, &len, sizeof(len), MSG_NOSIGNAL) < 0)
        throw std::runtime_error("Client: send length failed");
    // Send payload
    size_t sent = 0;
    while (sent < data.size()) {
        ssize_t n = ::send(fd, data.data() + sent, data.size() - sent,
                           MSG_NOSIGNAL);
        if (n <= 0)
            throw std::runtime_error("Client: send data failed");
        sent += static_cast<size_t>(n);
    }
}

bool Client::_recvExact(int fd, uint8_t* buf, size_t n)
{
    size_t received = 0;
    while (received < n) {
        ssize_t r = ::recv(fd, buf + received, n - received, 0);
        if (r <= 0)
            return false;
        received += static_cast<size_t>(r);
    }
    return true;
}

} // namespace net
} // namespace libcpp
