#include "platform.hpp"

#include "tasks/TaskMain.hpp"

#include "MessageAck.hpp"

bool TaskMain::setup()
{
    bool success = imu.setup();
    success &= ml.add_listener(cmd);
    return success;
}

void TaskMain::loop()
{
    static uint16_t iter = 0;
    ++iter;

    tm.message.t_start = Clock::get_usec<uint32_t>();

    // Update sensor data.
    imu.collect();

    // Check for messages. Process everything available, we'll
    // let this task overrun.
    ml.poll(reader);
    while (true)
    {
        auto mid = ml.process_next();
        if (mid <= -1) break;
        process_message(mid);
    }

    // Send sensor telemetry.
    imu.write_telemetry(writer);

    // Send task TM.
    tm.message.t_end = Clock::get_usec<uint32_t>();
    tm.serialize();
    writer.write(tm.buffer);
}

void TaskMain::process_message(const uint16_t mid)
{
    bool invalid_checksum = false;

    // Command message?
    if (mid == Command::message_id)
    {
        // Extract command.
        if (cmd.valid_checksum())
        {
            auto & msg = cmd.deserialize();

            switch (msg.command)
            {
                case Command::NoOp:
                default:
                    break;
            }

            cmd.message.ack = MessageAck::OK;
        }
        else
        {
            invalid_checksum = true;
            cmd.message.ack = MessageAck::Resend;
        }

        // Send acknowledgement.
        cmd.serialize();
        writer.write(cmd);
    }

    // Invalid checksum?
    if (invalid_checksum)
    {
        tm.message.invalid_checksum++;
    }
}
