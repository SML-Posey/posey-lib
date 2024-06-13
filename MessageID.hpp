#pragma once

#include <stdint.h>

namespace MessageID {
// Sensors.
constexpr uint8_t IMUData = 1;         // 0x01
constexpr uint8_t BLEData = 2;         // 0x02
constexpr uint8_t FlashBlockData = 3;  // 0x03

// Control.
constexpr uint8_t Command = 40;  // 0x28

constexpr uint8_t DataSummary = 50;  // 0x32
constexpr uint8_t DataPacket = 51;   // 0x33

// TM from flash.
constexpr uint8_t FlashBlockTM = 100;  // 0x64

// Tasks.
constexpr uint8_t Task = 200;       // 0xC8
constexpr uint8_t TaskWatch = 201;  // 0xC9
constexpr uint8_t TaskWaist = 202;  // 0xCA
constexpr uint8_t TaskRing = 203;   // 0xCB

// Download.
constexpr uint8_t DownloadSpecs = 250;  // 0xFA
constexpr uint8_t StorageSpecs = 251;   // 0xFB

}  // namespace MessageID
