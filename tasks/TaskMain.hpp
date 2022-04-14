#pragma once

#include "MessageID.hpp"

#include "platform/BaseTask.hpp"
#include "platform/sensors/BaseIMU.hpp"
#include "platform/sensors/BaseBLE.hpp"

#include "platform/io/BaseMessageReader.hpp"
#include "platform/io/BaseMessageWriter.hpp"
#include "platform/io/MessageListener.hpp"
#include "platform/io/BufferMessagePair.hpp"

#include "control/Command.hpp"

class TaskMainTelemetry
{
    public:
        static constexpr uint8_t message_id = MessageID::TaskMain;
        static constexpr uint16_t MessageSize =
            1       // Message ID
            + 4*2   // Times
            + 1*1;  // Invalid checksum.
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer & buffer) const
        {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer
                .write_syncword()
                .write(message_id)
                .write(t_start).write(t_end)
                .write(invalid_checksum)
                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(t_start).read(t_end)
                .read(invalid_checksum);
            // Checksum.
            return true;
        }

    public:
        uint32_t t_start = 0;
        uint32_t t_end = 0;

        uint8_t invalid_checksum = 0;
};

class TaskMain : public BaseTask
{
    public:
        constexpr static float pi = 3.1415926535897;
        constexpr static float dt = 1.0/25.0;

    public:
        TaskMain(
            BaseIMU & imu,
            BaseBLE & ble,
            BaseMessageReader & reader,
            BaseMessageWriter & writer) :
            imu(imu), ble(ble), reader(reader), writer(writer) {}

        bool setup() override;
        void loop() override;

    protected:
        void process_message(const uint16_t mid);

    protected:
        BaseIMU & imu;
        BaseBLE & ble;

        BaseMessageReader & reader;
        BaseMessageWriter & writer;
        MessageListener<1, 50> ml;

        // Messages handled.
        BufferMessagePair<TaskMainTelemetry> tm;
        BufferMessagePair<Command> cmd; // <->
};
