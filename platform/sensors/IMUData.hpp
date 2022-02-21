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
            + 4*9;  // 9-DoF IMU
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
                .write(Ax).write(Ay).write(Az)
                .write(Gx).write(Gy).write(Gz)
                .write(Mx).write(My).write(Mz)
                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(time)
                .read(Ax).read(Ay).read(Az)
                .read(Gx).read(Gy).read(Gz)
                .read(Mx).read(My).read(Mz);
            // Checksum.
            return true;
        }

    public:
        uint32_t time;

        float Ax, Ay, Az;
        float Gx, Gy, Gz;
        float Mx, My, Mz;
};
