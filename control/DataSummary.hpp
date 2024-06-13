#pragma once

#include "MessageID.hpp"
#include "platform/io/BufferSerializer.hpp"

class DataSummary {
    public:
        static constexpr int MaxDatetimeSize = 30;

        static constexpr uint8_t message_id = MessageID::DataSummary;
        static constexpr uint16_t MessageSize =
            1                      // message_id
            + 1 * MaxDatetimeSize  // datetime
            + 2 * 4                // Start / end time (32-bit)
            + 1 * 4                // Data bytes (32-bit)
            ;
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer& buffer) const {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer.write_syncword()
                .write(message_id)
                .write(datetime)
                .write(start_ms)
                .write(end_ms)
                .write(bytes)
                .write_checksum();
        }

        bool deserialize(Buffer& buffer) {
            buffer.rewind();
            buffer.read<uint16_t>();  // Syncword.
            buffer.read<uint8_t>();   // Message ID.
            buffer.read(datetime).read(start_ms).read(end_ms).read(bytes);
            // Checksum.
            return true;
        }

    public:
        uint8_t datetime[MaxDatetimeSize];
        uint32_t start_ms;
        uint32_t end_ms;
        uint32_t bytes;
};
