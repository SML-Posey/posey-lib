#pragma once

#include "MessageID.hpp"

#include "platform/io/BufferSerializer.hpp"

class IMUData
{
    public:
        static constexpr uint8_t message_id = MessageID::IMUData;
        static constexpr uint16_t MessageSize =
            1       // Message ID
            + 4*1   // Time
            + 4*3   // 3-DoF Accel
            + 4*4   // Quat + Acc
            ;
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer & buffer) const
        {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer
                .write_syncword()
                .write(message_id)
                .write(time_ms)

                .write(Ax).write(Ay).write(Az)
                .write(Qi).write(Qj).write(Qk).write(Qr)

                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(time_ms)
                .read(Ax).read(Ay).read(Az)
                .read(Qi).read(Qj).read(Qk).read(Qr);
            // Checksum.
            return true;
        }

    public:
        uint32_t time_ms;

        float Ax = 0, Ay = 0, Az = 0;
        float Qi = 0, Qj = 0, Qk = 0, Qr = 0;
};
