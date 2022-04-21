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
            + 4*4   // IRQ counts
            + 4*9   // 9-DoF IMU
            + 4*5;  // Pose quaternion + acc.
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
                .write(An).write(Gn).write(Mn).write(Qn)
                .write(Ax).write(Ay).write(Az)
                .write(Gx).write(Gy).write(Gz)
                .write(Mx).write(My).write(Mz)
                .write(Qi).write(Qj).write(Qk).write(Qr).write(Qacc)
                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(time)
                .read(An).read(Gn).read(Mn).read(Qn)
                .read(Ax).read(Ay).read(Az)
                .read(Gx).read(Gy).read(Gz)
                .read(Mx).read(My).read(Mz)
                .read(Qi).read(Qj).read(Qk).read(Qr).read(Qacc);
            // Checksum.
            return true;
        }

    public:
        uint32_t time;

        uint32_t An = 0, Gn = 0, Mn = 0, Qn = 0;
        float Ax = 0, Ay = 0, Az = 0;
        float Gx = 0, Gy = 0, Gz = 0;
        float Mx = 0, My = 0, Mz = 0;
        float Qi = 0, Qj = 0, Qk = 0, Qr = 0, Qacc = 0;
};
