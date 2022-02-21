#pragma once

#include <stdint.h>
#include <stddef.h>

class BufferCopyCallback
{
    public:
        virtual ~BufferCopyCallback() {}

        virtual void reset() = 0;
        virtual void copy(
            const uint8_t * buffer,
            uint16_t size = 0,
            const bool last = true) = 0;
};

template <uint16_t N>
class BufferSerializer
{
    public:
        static constexpr uint8_t syncword[2] = {0xca, 0xfe};

    public:
        class CopyCallback : public BufferCopyCallback
        {
            public:
                CopyCallback(BufferSerializer<N> & bs) : bs(bs) {}

                void reset() override { bs.reset(); }

                void copy(
                    const uint8_t * const buffer,
                    uint16_t size = 0,
                    const bool last = true) override
                {
                    if (size == 0) size = bs.capacity();
                    if (last) --size;
                    for (uint16_t i = 0; i < size; ++i)
                        bs.write(buffer[i]);
                    if (last)
                    {
                        bs.write_checksum();
                        bs.rewind();
                    }
                }

            private:
                BufferSerializer<N> & bs;
        };

    public:
        CopyCallback copy_callback;

    public:
        BufferSerializer() : copy_callback(*this) { reset(); }

        void reset()
        {
            cursor = 0;
            bytes_used = 0;
            checksum = 0;
        }

        inline void rewind() { cursor = 0; }

        template <typename T>
        BufferSerializer<N> & write(const T & elem)
        {
            // Too big?
            if ((cursor + sizeof(T)) > capacity())
                return *this;

            const uint8_t * data = reinterpret_cast<const uint8_t * >(&elem);
            for (size_t bi = 0; bi < sizeof(T); ++bi)
            {
                buffer[cursor + bi] = data[bi];
                checksum ^= data[bi];
            }
            cursor += sizeof(T);
            if (cursor > bytes_used)
                bytes_used = cursor;

            return *this;
        }

        template <typename T>
        BufferSerializer<N> & write(
            const T * data,
            const uint16_t nelem)
        {
            auto size = sizeof(T)*nelem;

            // Too big?
            if ((cursor + size) > capacity())
                return *this;

            for (auto i = 0; i < nelem; ++i)
                write(data[i]);
            return *this;
        }

        inline BufferSerializer<N> & write_syncword()
        {
            return write(syncword);
        }

        inline void write_checksum()
        {
            buffer[cursor] = checksum;
            ++bytes_used;
            ++cursor;
        }

        template <typename T>
        BufferSerializer<N> & read(T & data)
        {
            if (used() >= sizeof(T))
            {
                data = *reinterpret_cast<const T *>(&buffer[cursor]);
                cursor += sizeof(T);
            }
            return *this;
        }

        template <typename T>
        BufferSerializer<N> & read(
            T * data,
            const uint16_t nelem)
        {
            auto size = sizeof(T)*nelem;

            // Enough data?
            if (used() >= size)
            {
                for (auto i = 0; i < nelem; ++i)
                    read(data[i]);
            }
            return *this;
        }

        template <typename T>
        T read()
        {
            T data;
            read<T>(data);
            return data;
        }

        inline auto capacity() const { return size; }
        inline auto used() const { return bytes_used; }
        inline const uint8_t * get_buffer() const
            { return &buffer[0]; }

        bool valid_checksum() const
        {
            uint8_t cs = 0;
            for (uint16_t bi = 0; bi < used() - 1; ++bi)
                cs ^= buffer[bi];
            return cs == buffer[used() - 1];
        }

        inline uint8_t get_checksum() const { return checksum; }

    private:
        uint8_t checksum = 0;
        static constexpr uint16_t size = N + sizeof(syncword) + sizeof(checksum);
        uint8_t buffer[size];
        uint16_t cursor = 0;
        uint16_t bytes_used = 0;
};
