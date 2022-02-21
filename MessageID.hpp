#pragma once

#include <stdint.h>

namespace MessageID
{
    // Sensors.
    constexpr uint8_t IMUData           = 1;

    // Control.
    constexpr uint8_t Command           = 40;

    // Tasks.
    constexpr uint8_t TaskMain          = 200;
    constexpr uint8_t TaskWatch         = 201;
    constexpr uint8_t TaskWaist         = 202;
    constexpr uint8_t TaskRing          = 203;
}
