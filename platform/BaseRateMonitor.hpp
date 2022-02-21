#pragma once

template <class Clock>
class BaseRateMonitor
{
    public:
        BaseRateMonitor() :
            last_time_nsec(0), elapsed_nsec(0) {}

        explicit BaseRateMonitor(const BaseRateMonitor & other) = delete;
        BaseRateMonitor & operator=(const BaseRateMonitor & other) = delete;

        explicit BaseRateMonitor(BaseRateMonitor && other) = delete;
        BaseRateMonitor && operator=(BaseRateMonitor && other) = delete;

        inline void reset()
        {
            last_time_usec = Clock::get_usec();
            elapsed_usec = 0;
        }

        inline long mark()
        {
            // Compare with current time.
            long time_usec = Clock::get_usec();
            long usec_diff = time_usec - last_time_usec;
            last_time_usec = time_usec;
            elapsed_usec += usec_diff;
            return usec_diff;
        }

        inline long get_elapsed_time() const { return elapsed_usec; }

    private:
        long last_time_usec;
        long elapsed_usec;
};
