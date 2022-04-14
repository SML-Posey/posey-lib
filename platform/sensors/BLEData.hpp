#pragma once

#include "MessageID.hpp"

#include "platform/io/BufferSerializer.hpp"

class BLEData
{
    public:
        static constexpr uint8_t ble_addr_len = 6;
        static constexpr uint8_t message_id = MessageID::BLEData;
        static constexpr uint16_t MessageSize =
            1       // Message ID
            + 4*1   // Time
            + ble_addr_len*1   // Bluetooth address
            + 1*1;  // RSSI.
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer & buffer) const
        {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer
                .write_syncword()
                .write(message_id)
                .write(time)
                .write(addr)
                .write(rssi)
                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(time)
                .read(addr)
                .read(rssi);
            // Checksum.
            return true;
        }

    public:
        uint32_t time;

        uint8_t addr[ble_addr_len];
        int8_t rssi;
};
