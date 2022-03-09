#pragma once

#include "platform/hardware/BaseHardwareInterface.hpp"
#include "platform/sensors/IMUData.hpp"

class BaseIMU : public BaseHardwareInterface<IMUData> {};
