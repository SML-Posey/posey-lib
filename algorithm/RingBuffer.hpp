#pragma once

#include <stdint.h>

template <typename T, uint16_t BufferSize>
class RingBuffer
{
    public:
        auto capacity() const { return BufferSize; }
        auto used() const { return bytes_used; }
        auto free() const { return capacity() - used(); }

        const T * get_buffer() const { return buffer; }

        uint16_t write_from(
            const T * const source,
            uint16_t size)
        {
            if (size > free())
                size = free();

            auto write_cursor = wrap(read_cursor + bytes_used);
            for (auto si = 0; si < size; ++si)
            {
                buffer[write_cursor] = source[si];
                inc_wrap(write_cursor);
            }
            bytes_used += size;

            return size;
        }

        uint16_t read_to(
            T * destination,
            uint16_t size)
        {
            if (size > used())
                size = used();
            
            for (auto di = 0; di < size; ++di)
            {
                destination[di] = buffer[read_cursor];
                inc_wrap(read_cursor);
            }
            bytes_used -= size;

            return size;
        }

    private:
        inline uint16_t wrap(const uint16_t idx)
            { return idx % BufferSize; }
        inline void inc_wrap(uint16_t & idx, const uint16_t inc = 1)
            { idx = wrap(idx + inc); }

    protected:
        uint8_t buffer[BufferSize];
        uint16_t bytes_used = 0;
        uint16_t read_cursor = 0;
};
