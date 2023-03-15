#pragma once

#include "MessageID.hpp"

#include "platform/io/BufferSerializer.hpp"
#include "platform/hardware/PeripheralConnection.hpp"

class TaskWaistTelemetry
{
    public:
        static constexpr uint8_t message_id = MessageID::TaskWaist;
        static constexpr uint16_t MessageSize =
            1       // Message ID
            + 4*1   // Task counter
            + 4*2   // Times
            + 1*1   // Invalid checksum
            + 1*1;  // Missed deadline
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer & buffer) const
        {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer
                .write_syncword()
                .write(message_id)
                .write(counter)
                .write(t_start).write(t_end)
                .write(invalid_checksum)
                .write(missed_deadline)
                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(counter)
                .read(t_start).read(t_end)
                .read(invalid_checksum)
                .read(missed_deadline);
            // Checksum.
            return true;
        }

    public:
        uint32_t counter = 0;

        uint32_t t_start = 0;
        uint32_t t_end = 0;

        uint8_t invalid_checksum = 0;
        uint8_t missed_deadline = 0;
};
