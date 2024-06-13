#pragma once

template <typename T>
class BufferMessagePair {
    public:
        bool valid_checksum() const { return buffer.valid_checksum(); }

        T& deserialize() {
            message.deserialize(buffer);
            return message;
        }

        void serialize() { message.serialize(buffer); }

    public:
        typename T::Buffer buffer;
        T message;
};
