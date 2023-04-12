#include "platform.hpp"

#include "tasks/TaskWatch.hpp"

#include "posey-platform/platform/io/NordicSAADACDriver.h"

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
        float Vbatt = read_Vbatt();
        if (Vbatt < 3.2) Vbatt = 3.2;
        if (Vbatt > 4.2) Vbatt = 4.2;
        tm.message.Vbatt = (read_Vbatt() - 3.2)/4.2*255;
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
