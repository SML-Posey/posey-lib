#pragma once

// Warning: Not made to be thread-safe. Read from a single thread and
// to from a single thread. This will drop packets if data is not read
// quickly enough.

template <typename T, int N>
class ElemRingBuffer {
    public:
        ElemRingBuffer() {}
        ~ElemRingBuffer() {}

        constexpr int capacity() const { return N; }
        int used() const { return to_read; }
        int free() const { return capacity() - used(); }

        void clear() {
            idx_read = idx_write = 0;
            to_read = 0;
        }

        T& get_write_buffer() { return buffer[idx_write]; }
        void commit_write() {
            ++to_read;
            idx_latest = idx_write;
            inc_wrap(idx_write);
            if (to_read >= capacity())
                dec_wrap(idx_write);
            if (to_read > capacity()) {
                --to_read;
                ++dropped;
            }

            ++total_writes;
        }

        void write(const T element) {
            get_write_buffer() = element;
            commit_write();
        }

        bool read_next(T& element) {
            if (data_available()) {
                element = buffer[idx_read];
                inc_wrap(idx_read);
                to_read -= 1;
                return true;
            } else
                return false;
        }

        // Not guaranteed to be good...
        T read_next() {
            T elem;
            if (!read_next(elem))
                read_latest(elem);
            return elem;
        }

        // No guarantees there's something valid in here in the beginning.
        void read_latest(T& element) const { element = buffer[idx_latest]; }
        T read_latest() const {
            T latest;
            read_latest(latest);
            return latest;
        }

        bool data_available() const { return used() > 0; }
        auto num_dropped() const { return dropped; }

    protected:
        static inline void inc_wrap(int& idx) { idx = (idx + 1) % N; }
        static inline void dec_wrap(int& idx) { idx = prev_wrap(idx); }
        static inline int prev_wrap(const int idx) {
            return idx ? (idx - 1) % N : N - 1;
        }

    private:
        T buffer[N];
        int idx_read = 0;
        int idx_write = 0;
        int idx_latest = 0;
        int to_read = 0;
        unsigned long total_writes = 0;
        unsigned long dropped = 0;
};
