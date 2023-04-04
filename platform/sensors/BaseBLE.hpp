#pragma once

#include "algorithm/ElemRingBuffer.hpp"
#include "platform/hardware/BaseHardwareInterface.hpp"
#include "platform/sensors/BLEData.hpp"


class BaseBLE : public BaseHardwareInterface<BLEData>
{
    public:
        bool collect() override
        {
            return true;
        }

        uint16_t write_telemetry(BaseMessageWriter & writer) override
        {
            // Write all elements in buffer.
            BLEData elem;
            uint16_t bytes_written = 0;
            while (ring_buffer.read_next(elem))
            {
                elem.serialize(buffer);
                bytes_written += writer.write(buffer);
            }
            return bytes_written;
        }

        void add_detection(
            const uint32_t time_ms,
            const uint8_t uuid[BLEData::uuid_len],
            const uint16_t major,
            const uint16_t minor,
            const int8_t power,
            const int8_t rssi)
        {
            if (ring_buffer.free() > 0)
            {
                BLEData & elem = ring_buffer.get_write_buffer();
                elem.time_ms = time_ms;
                for (int i = 0; i < BLEData::uuid_len; ++i)
                    elem.uuid[i] = uuid[i];
                elem.major = major;
                elem.minor = minor;
                elem.power = power;
                elem.rssi = rssi;
                ring_buffer.commit_write();
            }
            else ++dropped;
        }

    public:
        ElemRingBuffer<BLEData, 10> ring_buffer;
        uint32_t dropped = 0;
};
