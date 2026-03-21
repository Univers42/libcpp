// data_buffer.cpp — DataBuffer non-template method implementations.

#include "libcpp/data/data_buffer.hpp"
#include <cstdint>
#include <cstring>

namespace libcpp {
namespace data {

DataBuffer::DataBuffer(const char* data, size_t size)
    : _data(data, data + size), _readCursor(0)
{
}

DataBuffer& DataBuffer::operator<<(const std::string& str)
{
    uint32_t len = static_cast<uint32_t>(str.size());
    *this << len;
    _data.insert(_data.end(), str.begin(), str.end());
    return *this;
}

DataBuffer& DataBuffer::operator>>(std::string& str)
{
    uint32_t len = 0;
    *this >> len;
    if (_readCursor + len > _data.size())
        throw std::runtime_error("DataBuffer: read past end of buffer");
    str.assign(_data.data() + _readCursor, len);
    _readCursor += len;
    return *this;
}

size_t DataBuffer::size() const { return _data.size(); }
bool   DataBuffer::empty() const { return _data.empty(); }

void DataBuffer::clear()
{
    _data.clear();
    _readCursor = 0;
}

const std::vector<char>& DataBuffer::raw() const { return _data; }

void   DataBuffer::resetCursor() { _readCursor = 0; }
size_t DataBuffer::cursor() const { return _readCursor; }

void DataBuffer::setRaw(const char* data, size_t size)
{
    _data.assign(data, data + size);
    _readCursor = 0;
}

void DataBuffer::setRaw(const uint8_t* data, size_t size)
{
    setRaw(reinterpret_cast<const char*>(data), size);
}

} // namespace data
} // namespace libcpp
