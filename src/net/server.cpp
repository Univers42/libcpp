// server.cpp — TCP Server implementation using POSIX sockets.

#include "libcpp/net/server.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>

namespace libcpp {
namespace net {

Server::Server() = default;

Server::~Server() { stop(); }

void Server::start(uint16_t port) {
  if (_running) throw std::runtime_error("Server: already running");

  _listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (_listenFd < 0) throw std::runtime_error("Server: socket() failed");

  int opt = 1;
  ::setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (::bind(_listenFd, reinterpret_cast<struct sockaddr*>(&addr),
             sizeof(addr)) < 0) {
    ::close(_listenFd);
    _listenFd = -1;
    throw std::runtime_error("Server: bind() failed");
  }

  if (::listen(_listenFd, SOMAXCONN) < 0) {
    ::close(_listenFd);
    _listenFd = -1;
    throw std::runtime_error("Server: listen() failed");
  }

  _running = true;
  _acceptThread = std::thread(&Server::_acceptLoop, this);
}

void Server::stop() {
  _running = false;
  if (_listenFd >= 0) {
    ::shutdown(_listenFd, SHUT_RDWR);
    ::close(_listenFd);
    _listenFd = -1;
  }

  // Close all client sockets
  {
    std::lock_guard<std::mutex> lock(_clientsMtx);
    for (auto& [id, fd] : _clients) {
      ::shutdown(fd, SHUT_RDWR);
      ::close(fd);
    }
    _clients.clear();
  }

  if (_acceptThread.joinable()) _acceptThread.join();

  for (auto& t : _clientThreads) {
    if (t.joinable()) t.join();
  }
  _clientThreads.clear();
}

void Server::sendTo(ClientId clientId, const Message& msg) {
  std::lock_guard<std::mutex> lock(_clientsMtx);
  auto it = _clients.find(clientId);
  if (it == _clients.end()) throw std::runtime_error("Server: unknown client");
  _sendRaw(it->second, msg.serialize());
}

void Server::sendToAll(const Message& msg) {
  auto data = msg.serialize();
  std::lock_guard<std::mutex> lock(_clientsMtx);
  for (auto& [id, fd] : _clients) {
    try {
      _sendRaw(fd, data);
    } catch (...) {
      // Skip clients that fail
    }
  }
}

void Server::registerHandler(MessageType type,
                             std::function<void(ClientId, Message&)> handler) {
  std::lock_guard<std::mutex> lock(_handlersMtx);
  _handlers[type] = std::move(handler);
}

void Server::onClientConnected(std::function<void(ClientId)> callback) {
  _onConnect = std::move(callback);
}

void Server::onClientDisconnected(std::function<void(ClientId)> callback) {
  _onDisconnect = std::move(callback);
}

bool Server::isRunning() const { return _running; }

// ── Private ───────────────────────────────────────────────────────────────

void Server::_acceptLoop() {
  while (_running) {
    struct sockaddr_in clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = ::accept(
        _listenFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &addrLen);
    if (clientFd < 0) {
      if (!_running) break;
      continue;  // transient error
    }

    ClientId id = _nextId++;
    {
      std::lock_guard<std::mutex> lock(_clientsMtx);
      _clients[id] = clientFd;
    }

    if (_onConnect) _onConnect(id);

    _clientThreads.emplace_back(&Server::_clientLoop, this, id, clientFd);
  }
}

void Server::_clientLoop(ClientId id, int clientFd) {
  while (_running) {
    uint32_t totalLen = 0;
    if (!_recvExact(clientFd, reinterpret_cast<uint8_t*>(&totalLen),
                    sizeof(totalLen)))
      break;

    std::vector<uint8_t> buf(totalLen);
    if (!_recvExact(clientFd, buf.data(), totalLen)) break;

    try {
      Message msg = Message::deserialize(buf);
      std::lock_guard<std::mutex> lock(_handlersMtx);
      auto it = _handlers.find(msg.type());
      if (it != _handlers.end()) it->second(id, msg);
    } catch (...) {
      // Drop malformed messages
    }
  }

  // Client disconnected
  {
    std::lock_guard<std::mutex> lock(_clientsMtx);
    auto it = _clients.find(id);
    if (it != _clients.end()) {
      ::close(it->second);
      _clients.erase(it);
    }
  }

  if (_onDisconnect) _onDisconnect(id);
}

void Server::_sendRaw(int fd, const std::vector<uint8_t>& data) {
  uint32_t len = static_cast<uint32_t>(data.size());
  if (::send(fd, &len, sizeof(len), MSG_NOSIGNAL) < 0)
    throw std::runtime_error("Server: send length failed");
  size_t sent = 0;
  while (sent < data.size()) {
    ssize_t n =
        ::send(fd, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
    if (n <= 0) throw std::runtime_error("Server: send data failed");
    sent += static_cast<size_t>(n);
  }
}

bool Server::_recvExact(int fd, uint8_t* buf, size_t n) {
  size_t received = 0;
  while (received < n) {
    ssize_t r = ::recv(fd, buf + received, n - received, 0);
    if (r <= 0) return false;
    received += static_cast<size_t>(r);
  }
  return true;
}

}  // namespace net
}  // namespace libcpp
