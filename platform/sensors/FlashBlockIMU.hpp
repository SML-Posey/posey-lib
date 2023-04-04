#pragma once

#include "MessageID.hpp"

#include "platform/io/BufferSerializer.hpp"

class FlashBlockIMU
{
    public:
        static constexpr uint8_t message_id = MessageID::FlashBlockTM;
        static constexpr uint8_t Samples = 8;
        static constexpr uint16_t MessageSize =
            1       // Message ID
            + 4*1   // Time
            + 1     // Vbatt
            + 1     // missed_deadlines
            + 3*4*Samples   // Accelerometer
            + 4*4*Samples   // Quaternion
            ;
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        bool add_sample(
            const uint32_t time,
            const uint8_t Vbatt,
            const uint8_t missed_deadlines,

            const float Ax,
            const float Ay,
            const float Az,

            const float Qs,
            const float Qi,
            const float Qj,
            const float Qk)
        {
            if (si == 0)
            {
                this->time = time;
                this->Vbatt = Vbatt;
                this->missed_deadlines = missed_deadlines;
            }

            this->Ax[si] = Ax;
            this->Ay[si] = Ay;
            this->Az[si] = Az;

            this->Qs[si] = Qs;
            this->Qi[si] = Qi;
            this->Qj[si] = Qj;
            this->Qk[si] = Qk;

            ++si;
            if (si == Samples) si = 0;
            return si == 0;
        }

        void serialize(Buffer & buffer) const
        {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer
                .write_syncword()
                .write(message_id)
                .write(time)
                .write(Vbatt)
                .write(missed_deadlines)

                .write(Ax).write(Ay).write(Az)
                .write(Qs).write(Qi).write(Qj).write(Qk)

                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(time)
                .read(Vbatt)
                .read(missed_deadlines)

                .read(Ax).read(Ay).read(Az)
                .read(Qs).read(Qi).read(Qj).read(Qk)
            // Checksum.
            return true;
        }

    public:
        // Counter.
        uint8_t si = 0;

        // Data.
        uint32_t time_ms;

        uint8_t Vbatt;
        uint8_t missed_deadlines;

        float Ax[Samples];
        float Ay[Samples];
        float Az[Samples];

        float Qs[Samples];
        float Qi[Samples];
        float Qj[Samples];
        float Qk[Samples];
};
