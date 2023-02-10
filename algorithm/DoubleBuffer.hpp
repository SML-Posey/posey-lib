#pragma once

#include <stdint.h>

template <typename T, uint16_t Sz>
class Buffer
{
    public:
        uint16_t write(const T * const src, uint16_t size)
        {
            auto max_write = space_free();
            if (size > max_write) size = max_write;
            for (uint16_t i = 0; i < size; ++i)
                buffer[write_cursor + i] = src[i];
            write_cursor += size;
            return size;
        }

        uint16_t capacity() const { return Sz; }
        uint16_t space_used() const { return write_cursor; }
        uint16_t space_free() const { return capacity() - space_used(); }
        bool is_clear() const { return space_free() == capacity(); }

        size_t used_bytes() const { return space_used()*sizeof(T); }
        const char * bytes() const { return reinterpret_cast<const char *>(buffer); }

        void reset() { write_cursor = 0; }

    public:
        T buffer[Sz];
        uint16_t write_cursor = 0;
};

template <typename T, int Sz>
class DoubleBuffer
{
    public:
        static constexpr int N = 2;

    public:
        struct WriteStatus
        {
            uint16_t bytes_written = 0;
            uint16_t bytes_lost = 0;
            bool buffer_switched = false;

            bool uncleared_buffer = false;
            uint16_t uncleared_buffer_dataloss = 0;

        };

    public:
        WriteStatus write(const T * const src, uint16_t size)
        {
            WriteStatus status;
            status.bytes_written = active_buffer().write(src, size);

            // Do we need to switch the buffer?
            status.buffer_switch = status.bytes_written < size;
            if (status.buffer_switch)
            {
                flip_buffer();

                // Check if the new buffer is clear. If not, there will be
                // some data loss.
                status.uncleared_buffer = !active_buffer().is_clear();
                if (status.uncleared_buffer)
                {
                    status.uncleared_buffer_dataloss = active_buffer().space_used();
                    active_buffer().reset();
                }

                status.bytes_written += active_buffer().write(
                    &src[status.bytes_written], size - status.bytes_written);
            }

            status.bytes_lost = size - status.bytes_written;

            return status;
        }

        void flip_buffer() { write_buffer = (write_buffer + 1) % 2; }
        Buffer<T, Sz> & active_buffer() const { return buffers[write_buffer]; }
        Buffer<T, Sz> & inactive_buffer() const { return buffers[(write_buffer + 1) % 2]; }

    public:
        Buffer<T, Sz> buffers[N];
        int write_buffer = 0;
};