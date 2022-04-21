#pragma once

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

        Td remaining()
        {
            Clock::set_usec(t1_us);
            return static_cast<Td>(period_us) - static_cast<Td>(t1_us) + static_cast<Td>(t0_us);
        }

        void delay_remaining()
        {
            Clock::set_usec(t1_us);
            auto us = t1_us - t0_us;
            if (us < period_us)
                Clock::delay_usec(period_us - us);
        }

        bool run()
        {
            delta_us = remaining();
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
