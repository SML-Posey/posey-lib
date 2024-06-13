#pragma once

#include <stdint.h>

namespace MessageAck {
// Standard acknowledgements.
constexpr uint8_t DontCare = 0x00;
constexpr uint8_t Expected = 0xA9;
constexpr uint8_t OK = 0xB8;
constexpr uint8_t Resend = 0xC7;
constexpr uint8_t Error = 0xD6;
constexpr uint8_t Working = 0xE5;

}  // namespace MessageAck
