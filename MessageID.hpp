#pragma once

#include <stdint.h>

namespace MessageID
{
    // Sensors.
    constexpr uint8_t IMUData           = 1;    // 0x01

    // Control.
    constexpr uint8_t Command           = 40;   // 0x28

    // Tasks.
    constexpr uint8_t TaskMain          = 200;  // 0xC8
    constexpr uint8_t TaskWatch         = 201;  // 0xC9
    constexpr uint8_t TaskWaist         = 202;  // 0xCA
    constexpr uint8_t TaskRing          = 203;  // 0xCB
}
