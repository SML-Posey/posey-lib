#pragma once

#include "MessageID.hpp"

#include "platform/io/BufferSerializer.hpp"

class FlashBlockData {
    public:
        static constexpr uint8_t message_id = MessageID::FlashBlockData;
        static constexpr uint16_t MessageSize = 1        // Message ID
                                                + 4 * 1  // Time
                                                + 1      // Slot
                                                + 6      // MAC
                                                + 1      // rssi
                                                + 2;     // block bytes
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer& buffer) const {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer.write_syncword()
                .write(message_id)
                .write(time_ms)
                .write(slot)
                .write(mac)
                .write(rssi)
                .write(block_bytes)
                .write_checksum();
        }

        bool deserialize(Buffer& buffer) {
            buffer.rewind();
            buffer.read<uint16_t>();  // Syncword.
            buffer.read<uint8_t>();   // Message ID.
            buffer.read(time_ms).read(slot).read(mac).read(rssi).read(
                block_bytes);
            // Checksum.
            return true;
        }

    public:
        uint32_t time_ms = 0;
        uint8_t slot = 0;
        uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
        int8_t rssi = 0;
        uint16_t block_bytes = 0;
};
