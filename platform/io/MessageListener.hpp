#pragma once

#include <stdint.h>

#include "platform/io/BaseMessageReader.hpp"
#include "platform/io/BufferMessagePair.hpp"
#include "platform/io/BufferSerializer.hpp"

template <uint8_t MaxListeners, uint16_t ReadBufferSize>
class MessageListener {
    public:
        bool add_listener(
            uint8_t id, uint16_t size, BufferCopyCallback* callback) {
            if (num_listeners < MaxListeners) {
                listeners[num_listeners] = {id, size, callback};
                ++num_listeners;
                return true;
            }
            return false;
        }

        template <typename T>
        bool add_listener(BufferMessagePair<T>& bmp) {
            return add_listener(
                bmp.message.message_id, bmp.buffer.capacity(),
                &bmp.buffer.copy_callback);
        }

        int32_t listener_index(const uint8_t message_id) const {
            for (auto li = 0; li < num_listeners; ++li)
                if (listeners[li].id == message_id)
                    return li;
            return -1;
        }

        auto num_messages() const { return messages; }
        auto num_ignored_messages() const { return ignored_messages; }
        auto num_skipped_bytes() const { return skipped_bytes; }

        auto capacity() const { return ReadBufferSize; }
        auto used() const { return bytes_used; }
        auto free() const { return capacity() - used(); }

        const uint8_t* get_buffer() const { return buffer; }

        uint16_t write(const uint8_t* const source, uint16_t size) {
            if (size > free())
                size = free();

            auto write_cursor = wrap(read_cursor + bytes_used);
            for (auto si = 0; si < size; ++si) {
                buffer[write_cursor] = source[si];
                inc_wrap(write_cursor);
            }
            bytes_used += size;

            return size;
        }

        uint16_t poll(BaseMessageReader& src) {
            uint16_t size_out = 0;

            auto sidx = wrap(read_cursor + bytes_used);
            auto eidx = sidx + free();
            auto eidx_1 = eidx > ReadBufferSize ? ReadBufferSize : eidx;
            auto size_1 = eidx_1 - sidx;
            size_out += src.read_to(&buffer[sidx], size_1);
            if ((size_out == size_1) && (eidx != eidx_1)) {
                auto eidx_2 = wrap(eidx);
                size_out += src.read_to(&buffer[0], eidx_2);
            }
            bytes_used += size_out;

            return size_out;
        }

        uint16_t poll(BaseMessageReader&& src) { return poll(src); }

        int16_t process_next() {
            // Find next syncword.
            while (bytes_used >= 4) {
                uint16_t i1 = read_cursor, i2 = wrap(read_cursor + 1);
                if (buffer[i1] != BufferSerializer<0>::syncword[0])
                    skip(1);
                else if (buffer[i2] != BufferSerializer<0>::syncword[1])
                    skip(2);
                else {
                    // Listening for this message?
                    uint8_t mid = buffer[wrap(read_cursor + 2)];
                    auto lidx = listener_index(mid);
                    if (lidx >= 0) {
                        // Found a message. Is it all here?
                        auto msize = listeners[lidx].size;
                        if (bytes_used >= msize) {
                            // Trigger callback, handling wrap-around if
                            // needed.
                            auto sidx = read_cursor;
                            auto eidx = read_cursor + msize;
                            auto eidx_1 =
                                eidx > ReadBufferSize ? ReadBufferSize : eidx;
                            listeners[lidx].callback->reset();
                            listeners[lidx].callback->copy(
                                &buffer[sidx], eidx_1 - sidx, eidx_1 == eidx);
                            if (eidx != eidx_1) {
                                auto eidx_2 = wrap(eidx);
                                listeners[lidx].callback->copy(
                                    &buffer[0], eidx_2, true);
                            }
                            consume(msize);
                            ++messages;
                            return mid;
                        } else {
                            // Wait for more data.
                            break;
                        }
                    } else {
                        // Not listening for this message.
                        skip(2);
                        ++ignored_messages;
                    }
                }
            }
            return -1;
        }

    public:
        inline uint16_t wrap(const uint16_t idx) {
            return idx % ReadBufferSize;
        }
        inline void inc_wrap(uint16_t& idx, const uint16_t inc = 1) {
            idx = wrap(idx + inc);
        }
        inline uint16_t consume(uint16_t size) {
            if (size > bytes_used)
                size = bytes_used;
            inc_wrap(read_cursor, size);
            bytes_used -= size;
            return size;
        }
        inline void skip(uint16_t size) {
            size = consume(size);
            skipped_bytes += size;
        }
        inline uint8_t read_byte() {
            if (bytes_used == 0)
                return 0;
            uint8_t byte = buffer[read_cursor];
            inc_wrap(read_cursor);
            return byte;
        }

    protected:
        struct Listener {
                uint8_t id = 0;
                uint16_t size = 0;
                BufferCopyCallback* callback = nullptr;
        };

    protected:
        Listener listeners[MaxListeners];
        uint8_t num_listeners = 0;

        uint8_t buffer[ReadBufferSize];
        uint16_t bytes_used = 0;
        uint16_t read_cursor = 0;

        uint16_t messages = 0;
        uint16_t ignored_messages = 0;
        uint16_t skipped_bytes = 0;
};
