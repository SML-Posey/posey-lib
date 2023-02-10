#pragma once

#include "platform/hardware/BaseHardwareInterface.hpp"
#include "platform/sensors/FlashBlockData.hpp"

class BaseFlashBlock : public BaseHardwareInterface<FlashBlockData> {
    public:
        bool collect() override { return true; }
};
