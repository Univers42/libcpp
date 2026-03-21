#pragma once
// message.hpp — Tagged message using DataBuffer as payload.
//
// A Message wraps a type ID and serialised DataBuffer.
// Allows type-safe deserialization through registered callbacks.

#include "libcpp/data/data_buffer.hpp"
#include <cstdint>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>

namespace libcpp {
namespace net {

using MessageType = uint64_t;

/// @brief Typed message with DataBuffer payload for network serialization.
class Message {
public:
    Message() = default;
    explicit Message(MessageType type);
    ~Message() = default;

    // Access the underlying buffer for writing payload data
    data::DataBuffer& buffer();
    const data::DataBuffer& buffer() const;

    // Message type for dispatch
    MessageType type() const;
    void setType(MessageType type);

    // Serialize the whole message (type + payload) into bytes
    std::vector<uint8_t> serialize() const;

    // Deserialize from raw bytes
    static Message deserialize(const std::vector<uint8_t>& data);
    static Message deserialize(const uint8_t* data, size_t len);

private:
    MessageType      _type = 0;
    data::DataBuffer _payload;
};

// ── MessageConsumer: type-to-callback dispatcher ──────────────────────────
/// @brief Routes incoming messages to registered type-specific handlers.
class MessageConsumer {
public:
    MessageConsumer() = default;
    ~MessageConsumer() = default;

    // Register a handler for a given message type
    void registerHandler(MessageType type, std::function<void(Message&)> handler);

    // Dispatch a message to its registered handler
    void consume(Message& msg);

private:
    std::map<MessageType, std::function<void(Message&)>> _handlers;
};

} // namespace net
} // namespace libcpp
