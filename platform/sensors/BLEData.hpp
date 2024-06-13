#pragma once

#include "MessageID.hpp"

#include "platform/io/BufferSerializer.hpp"

class BLEData {
    public:
        static constexpr uint8_t uuid_len = 16;
        static constexpr uint8_t message_id = MessageID::BLEData;
        static constexpr uint16_t MessageSize = 1               // Message ID
                                                + 4 * 1         // Time
                                                + uuid_len * 1  // UUID
                                                + 2 * 2         // Major, minor
                                                + 1 * 2;        // Power, RSSI.
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer& buffer) const {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer.write_syncword()
                .write(message_id)
                .write(time_ms)
                .write(uuid)
                .write(major)
                .write(minor)
                .write(power)
                .write(rssi)
                .write_checksum();
        }

        bool deserialize(Buffer& buffer) {
            buffer.rewind();
            buffer.read<uint16_t>();  // Syncword.
            buffer.read<uint8_t>();   // Message ID.
            buffer.read(time_ms)
                .read(uuid)
                .read(major)
                .read(minor)
                .read(power)
                .read(rssi);
            // Checksum.
            return true;
        }

    public:
        uint32_t time_ms = 0;

        uint8_t uuid[uuid_len];
        uint16_t major = 0;
        uint16_t minor = 0;
        int8_t power = 0;
        int8_t rssi = 0;
};
