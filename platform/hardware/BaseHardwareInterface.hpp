#pragma once

#include <stdint.h>

#include "platform/io/BaseMessageWriter.hpp"

template <class DataType>
class BaseHardwareInterface {
    public:
        virtual ~BaseHardwareInterface() = default;

        virtual bool setup() { return true; }
        virtual bool collect() = 0;
        virtual uint16_t write_telemetry(BaseMessageWriter& writer) {
            data.serialize(buffer);
            return writer.write(buffer);
        }

        virtual void serialize() { data.serialize(buffer); }

    public:
        DataType data;
        typename DataType::Buffer buffer;
};
