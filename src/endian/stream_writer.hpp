// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "detail/stream.hpp"

namespace endian
{
/// The stream_writer provides a stream-like interface for writing to a
/// fixed size buffer. All complexity regarding endianness is encapsulated.
template <typename EndianType,
          typename parent = detail::stream<detail::non_const_stream>>
class stream_writer : public parent
{
public:
    /// Creates an endian stream on top of a pre-allocated buffer of the
    /// specified size.
    ///
    /// @param data a data pointer to the buffer
    /// @param size the size of the buffer in bytes
    template <typename... Args>
    stream_writer(Args... args) noexcept : parent(args...)
    {
    }

    /// Writes a Bytes-sized integer to the stream.
    ///
    /// @param value the value to write.
    template <uint8_t Bytes, class ValueType>
    void write_bytes(ValueType value) noexcept
    {
        parent::space(Bytes);

        EndianType::template put_bytes<Bytes>(value, this->remaining_data());
        parent::skip(Bytes);
    }

    /// Writes a Bytes-sized integer to the stream.
    ///
    /// @param value the value to write.
    template <class ValueType>
    void write(ValueType value) noexcept
    {
        parent::space(sizeof(ValueType));

        write_bytes<sizeof(ValueType), const ValueType>(value);
    }

    /// Writes the raw bytes represented by the storage::const_storage
    /// object to the stream.
    ///
    /// Note, that this function is provided only for convenience and
    /// it does not perform any endian conversions.
    ///
    /// @param data Pointer to the data, to be written to the stream.
    /// @param size Number of bytes from the data pointer.
    void write(const uint8_t* data, std::size_t size) noexcept
    {
        if (size)
        {
            parent::space(size);

            std::copy_n(data, size, this->remaining_data());
            parent::skip(size);
        }
    }
    void write(const std::vector<uint8_t>& v) noexcept
    {
        write(v.data(), v.size());
    }
    void write(const char* d) noexcept
    {
        write(reinterpret_cast<const uint8_t*>(d), strlen(d));
    }
    template <size_t N>
    void write(std::array<uint8_t, N> data) noexcept
    {
        write(&data[0], N);
    }

    /// Operator for writing given value to the end of the stream.
    ///
    /// @param value the value to write.
    template <typename ValueType>
    stream_writer<EndianType>& operator<<(ValueType value)
    {
        write(value);
        return *this;
    }
};
struct growing_stream
{
    std::vector<uint8_t> data;
    size_t off = 0;
    void skip(std::size_t bytes_to_skip) noexcept
    {
        off += bytes_to_skip;
    }
    uint8_t* remaining_data()
    {
        return &data.at(off);
    }
    void space(std::size_t n)
    {
        if (data.size() < (off + n))
            data.resize(off + n);
    }
};
}
