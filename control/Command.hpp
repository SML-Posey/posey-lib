#pragma once

#include "MessageID.hpp"
#include "platform/io/BufferSerializer.hpp"

class Command {
    public:
        static constexpr int MaxPayload = 60;

        enum CommandType {
            Configure = 0x01,          // Configure(params)
            ConnectPeripheral = 0x02,  // ConnectPeripheral(device, slot)
            Reboot = 0x03,             // Reboot()

            GetDataSummary = 0x10,  // -> Collection date/time, size.
            DownloadData = 0x11,    // DownloadData() -> Download packets
            FullFlashErase = 0x12,  //

            StartCollecting = 0x21,  // StartCollecting(date/time)
            StopCollecting = 0x22,   // StopCollecting

            NoOp = 0xFF
        };

        static const char* Command_to_string(const CommandType cmd) {
            switch (cmd) {
                case Configure:
                    return "Configure";
                case ConnectPeripheral:
                    return "ConnectPeripheral";
                case Reboot:
                    return "Reboot";

                case GetDataSummary:
                    return "GetDataSummary";
                case DownloadData:
                    return "DownloadData";
                case FullFlashErase:
                    return "FullFlashErase";

                case StartCollecting:
                    return "StartCollecting";
                case StopCollecting:
                    return "StopCollecting";

                case NoOp:
                    return "NoOp";

                default:
                    return "Unknown";
            }
        }

        static const char* Command_to_string(const uint8_t cmd) {
            return Command_to_string(static_cast<CommandType>(cmd));
        }

    public:
        static constexpr uint8_t message_id = MessageID::Command;
        static constexpr uint16_t MessageSize = 1                 // message_id
                                                + 1               // command
                                                + 1 * MaxPayload  // payload
                                                + 1               // ACK
            ;
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer& buffer) const {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer.write_syncword()
                .write(message_id)
                .write(command)
                .write(payload)
                .write(ack)
                .write_checksum();
        }

        bool deserialize(Buffer& buffer) {
            buffer.rewind();
            buffer.read<uint16_t>();  // Syncword.
            buffer.read<uint8_t>();   // Message ID.
            buffer.read(command).read(payload).read(ack);
            // Checksum.
            return true;
        }

        const char* command_str() const { return Command_to_string(command); }

    public:
        uint8_t command;
        uint8_t payload[MaxPayload];
        uint8_t ack;
};
