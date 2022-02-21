#pragma once

#include <stdint.h>

#include "platform/io/BufferSerializer.hpp"
#include "platform/io/BufferMessagePair.hpp"

class BaseMessageWriter
{
    public:
        virtual ~BaseMessageWriter() {}

        virtual uint16_t write(
            const uint8_t * buffer,
            const uint16_t size) = 0;

        template <uint16_t N>
        inline uint16_t write(const BufferSerializer<N> & bs)
        {
            return write(bs.get_buffer(), bs.used());
        }

        template <class T>
        inline uint16_t write(const BufferMessagePair<T> & bmp)
        {
            return write(bmp.buffer);
        }
};
