#pragma once

#include <stdint.h>

#include "platform/io/BufferSerializer.hpp"
#include "platform/io/BufferMessagePair.hpp"

class BaseMessageWriter
{
    public:
        static const bool SendNow = true;
        
    public:
        virtual ~BaseMessageWriter() {}

        virtual uint16_t write(
            const uint8_t * buffer,
            const uint16_t size,
            const bool immediate = false) = 0;

        template <uint16_t N>
        inline uint16_t write(
            const BufferSerializer<N> & bs,
            const bool immediate = false)
        {
            return write(bs.get_buffer(), bs.used(), immediate);
        }

        template <class T>
        inline uint16_t write(
            const BufferMessagePair<T> & bmp,
            const bool immediate=false)
        {
            return write(bmp.buffer, immediate);
        }
};
