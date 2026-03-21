// message.cpp — Message + MessageConsumer implementation.

#include "libcpp/net/message.hpp"
#include <cstring>

namespace libcpp {
namespace net {

// ── Message ───────────────────────────────────────────────────────────────

Message::Message(MessageType type) : _type(type) {}

data::DataBuffer& Message::buffer() { return _payload; }
const data::DataBuffer& Message::buffer() const { return _payload; }

MessageType Message::type() const { return _type; }
void Message::setType(MessageType type) { _type = type; }

std::vector<uint8_t> Message::serialize() const
{
    // Layout: [uint64_t type][uint32_t payloadSize][payload bytes...]
    const auto& raw = _payload.raw();
    uint32_t    payloadSize = static_cast<uint32_t>(raw.size());
    size_t      totalSize   = sizeof(_type) + sizeof(payloadSize) + raw.size();

    std::vector<uint8_t> out(totalSize);
    size_t               off = 0;

    std::memcpy(out.data() + off, &_type, sizeof(_type));
    off += sizeof(_type);
    std::memcpy(out.data() + off, &payloadSize, sizeof(payloadSize));
    off += sizeof(payloadSize);
    if (!raw.empty())
        std::memcpy(out.data() + off, raw.data(), raw.size());
    return out;
}

Message Message::deserialize(const std::vector<uint8_t>& data)
{
    return deserialize(data.data(), data.size());
}

Message Message::deserialize(const uint8_t* data, size_t len)
{
    constexpr size_t headerSize = sizeof(MessageType) + sizeof(uint32_t);
    if (len < headerSize)
        throw std::runtime_error("Message::deserialize: data too short");

    Message   msg;
    size_t    off = 0;

    std::memcpy(&msg._type, data + off, sizeof(msg._type));
    off += sizeof(msg._type);

    uint32_t payloadSize = 0;
    std::memcpy(&payloadSize, data + off, sizeof(payloadSize));
    off += sizeof(payloadSize);

    if (len < headerSize + payloadSize)
        throw std::runtime_error("Message::deserialize: truncated payload");

    // Load payload into the DataBuffer
    if (payloadSize > 0) {
        std::vector<uint8_t> payload(data + off, data + off + payloadSize);
        // Write raw bytes through operator<< for each byte? No — just set raw.
        // DataBuffer doesn't expose a raw-set, so we stream in blocks.
        // Actually let's just push raw bytes directly
        for (uint32_t i = 0; i < payloadSize; ++i)
            msg._payload << payload[i];
    }
    msg._payload.resetCursor();
    return msg;
}

// ── MessageConsumer ───────────────────────────────────────────────────────

void MessageConsumer::registerHandler(MessageType type,
                                      std::function<void(Message&)> handler)
{
    _handlers[type] = std::move(handler);
}

void MessageConsumer::consume(Message& msg)
{
    auto it = _handlers.find(msg.type());
    if (it == _handlers.end())
        throw std::runtime_error("MessageConsumer: no handler for message type");
    it->second(msg);
}

} // namespace net
} // namespace libcpp
