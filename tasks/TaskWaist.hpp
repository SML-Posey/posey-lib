#pragma once

#include "MessageID.hpp"

#include "platform/BaseTask.hpp"
#include "platform/sensors/BaseIMU.hpp"
#include "platform/sensors/BaseBLE.hpp"

#include "platform/io/BaseMessageReader.hpp"
#include "platform/io/BaseMessageWriter.hpp"
#include "platform/io/MessageListener.hpp"
#include "platform/io/BufferMessagePair.hpp"

#include "platform/hardware/PeripheralConnection.hpp"

#include "control/Command.hpp"

#include "platform/tasks/TaskWaistTelemetry.hpp"

class TaskWaist : public BaseTask
{
    public:
        TaskWaist(
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

        PeripheralConnection peripherals[4];

        BaseMessageReader & reader;
        BaseMessageWriter & writer;
        MessageListener<1, 50> ml;

        // Messages handled.
        BufferMessagePair<TaskWaistTelemetry> tm;
        BufferMessagePair<Command> cmd; // <->
};
