#include "platform.hpp"

#include "tasks/TaskWaist.hpp"

#include "MessageAck.hpp"

#include <zephyr/logging/log.h>

#define LOG_MODULE_NAME posey_task_hub
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

bool TaskWaist::setup()
{
    bool success = imu.setup();
    success &= ble.setup();
    success &= ml.add_listener(cmd);
    return success;
}

void TaskWaist::loop()
{
    static const uint32_t max_loop_time = 1e6/50;
    static uint32_t iter = 0;

    tm.message.t_start = Clock::get_usec<uint32_t>();
    tm.message.counter++;

    // For each active connection, collect IMU sensor data and RSSI.
    static const int MaxPeripherals = 4;
    for (int i = 0; i < MaxPeripherals; ++i)
    {
        //if (peripherals[i].collect_data())
        {
            // Store.
        }
    }

    // // Update sensor data.
    // imu.collect();
    // ble.collect(); // Does nothing.

    // Check for messages. Process everything available, we'll
    // let this task overrun.
    ml.poll(reader);
    while (true)
    {
        auto mid = ml.process_next();
        if (mid <= -1) break;
        process_message(mid);
    }

    // Send task TM.
    if (iter % 50 == 0)
    {
        tm.serialize();
        writer.write(tm.buffer);
    }
    ++iter;

    // Update missed deadlines if needed.
    tm.message.t_end = Clock::get_usec<uint32_t>();
    if (tm.message.t_end - tm.message.t_start > max_loop_time)
    {
        tm.message.missed_deadline++;
    }
}

void TaskWaist::process_message(const uint16_t mid)
{
    bool invalid_checksum = false;

    // Command message?
    if (mid == Command::message_id)
    {
        // Extract command.
        if (cmd.valid_checksum())
        {
            auto & msg = cmd.deserialize();

            LOG_INF("Received %s command", msg.command_str());

            switch (msg.command)
            {
                case Command::Configure:
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
