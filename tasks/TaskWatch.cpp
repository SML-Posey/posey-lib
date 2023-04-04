#include "platform.hpp"

#include "tasks/TaskWatch.hpp"

bool TaskWatch::setup()
{
    bool success = imu.setup();
    return success;
}

void TaskWatch::loop()
{
    static uint32_t loop_time = 1e3/50;
    static uint32_t iter = 0;

    tm.message.t_start_ms = Clock::get_msec<uint32_t>();

    // Update sensor data.
    imu.collect();

    // Send sensor telemetry.
    imu.write_telemetry(writer);

    // Send task TM at 1Hz.
    if (iter % 50 == 0)
    {
        tm.serialize();
        writer.write(tm.buffer, writer.SendNow);
    }
    ++iter;
    
    tm.message.t_end_ms = Clock::get_msec<uint32_t>();
    if (tm.message.t_end_ms - tm.message.t_start_ms > loop_time)
    {
        tm.message.missed_deadline++;
    }
}
