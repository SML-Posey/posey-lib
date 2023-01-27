#pragma once

#include <stdint.h>

class PeripheralConnection
{
    public:
        static const int MaxPeripherals = 4;
        enum Slot : uint8_t
        {
            Waist = 0,
            LeftWrist = 1,
            RightWrist = 2,
            Ring = 3
        };

        enum ConnectionStatus : uint8_t
        {
            Disconnected    = 0,
            Connected       = 1
        };

    public:
        virtual ~PeripheralConnection() {}

        virtual bool connect() = 0;
        virtual void disconnect() = 0;
        bool is_connected() const { return status == Connected; }

        virtual bool collect_data() = 0;

    public:
        char name[10] = "";
        ConnectionStatus status = Disconnected;
};
