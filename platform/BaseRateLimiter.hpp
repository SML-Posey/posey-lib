#pragma once

#include <limits>

template <class Clock, class Tp, class Td>
class BaseRateLimiter
{
    protected:
        BaseRateLimiter(const Tp period_us) :
            period_us(period_us), t0_us(0), t1_us(0), delta_us(0)
        {
            Clock::set_usec(t0_us);
        }

    public:
        static auto fromPeriod(const Tp period_us)
        {
            return BaseRateLimiter<Clock, Tp, Td>(period_us);
        }
        static auto fromHz(const float Hz)
        {
            return BaseRateLimiter<Clock, Tp, Td>(
                static_cast<Tp>(1.0e6/Hz));
        }

        void start()
        {
            reset();
        }

        void reset()
        {
            Clock::set_usec(t0_us);
        }

        template <typename T>
        T remaining()
        {
            Clock::set_usec(t1_us);
            Tp dt = t1_us - t0_us;
            if (t1_us < t0_us)
                dt = std::numeric_limits<Tp>::max() - t1_us + t0_us;
            if (period_us < dt)
                return static_cast<T>(0);
            else return static_cast<T>(period_us - dt);
        }

        void delay_remaining()
        {
            Tp remaining_us = remaining<Tp>();
            if (remaining_us > 0)
                Clock::delay_usec(remaining_us);
        }

        bool run()
        {
            delta_us = remaining<Td>();
            if (delta_us <= 0)
            {
                t0_us = t1_us;
                return true;
            } else return false;
        }

        const Clock & get_clock() const { return clock; }

        Td last_delta_usec() const { return delta_us; }
        Tp period_usec() const { return period_us; }

    private:
        Clock clock;
        Tp period_us = 0;
        Tp t0_us = 0, t1_us = 0;
        Td delta_us = 0;
};
