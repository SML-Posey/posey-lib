#include "platform.hpp"

#include "tasks/TaskWatch.hpp"

bool TaskWatch::setup()
{
    bool success = imu.setup();
    return success;
}

void TaskWatch::loop()
{
    static uint32_t loop_time = 1e6/50;
    tm.message.t_start = Clock::get_usec<uint32_t>();
    tm.message.counter++;

    // Update sensor data.
    imu.collect();

    // Send sensor telemetry.
    imu.write_telemetry(writer);

    // Send task TM.
    tm.serialize();
    writer.write(tm.buffer);
    tm.message.t_end = Clock::get_usec<uint32_t>();
    if (tm.message.t_end - tm.message.t_start > loop_time)
    {
        tm.message.missed_deadline++;
    }
}
