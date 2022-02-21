#pragma once

#include <stdint.h>

class BaseMessageReader
{
    public:
        virtual ~BaseMessageReader() {}

        virtual uint16_t available() = 0;
        virtual uint16_t read_to(
            uint8_t * dst_buffer,
            const uint16_t max_size) = 0;
};
