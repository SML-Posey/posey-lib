#pragma once

#include "MessageID.hpp"
#include "platform/io/BufferSerializer.hpp"

class Command
{
    public:
        enum Commands
        {
            NoOp,           //
        };

    public:
        static constexpr uint8_t message_id = MessageID::Command;
        static constexpr uint16_t MessageSize =
            1           // message_id
            + 1         // command
            + 4*3       // args
            + 1         // ACK
            ;
        typedef BufferSerializer<MessageSize> Buffer;

    public:
        void serialize(Buffer & buffer) const
        {
            static auto message_id = this->message_id;
            buffer.reset();
            buffer
                .write_syncword()
                .write(message_id)
                .write(command)
                .write(arg1).write(arg2).write(arg3)
                .write(ack)
                .write_checksum();
        }

        bool deserialize(Buffer & buffer)
        {
            buffer.rewind();
            buffer.read<uint16_t>(); // Syncword.
            buffer.read<uint8_t>();  // Message ID.
            buffer
                .read(command)
                .read(arg1).read(arg2).read(arg3)
                .read(ack);
            // Checksum.
            return true;
        }

    public:
        uint8_t command;

        float arg1;
        float arg2;
        float arg3;

        uint8_t ack;
};
