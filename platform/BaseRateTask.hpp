#pragma once

#include "platform/BaseTask.hpp"

template <class Limiter>
class BaseRateTask
{
    public:
        BaseRateTask(BaseTask & task, const float Hz) :
            task(task), limiter(Limiter::fromHz(Hz)),
            executions(0) {}

        bool setup() { return task.setup(); }

        bool start()
        {
            limiter.start();
            return true;
        }

        auto num_executions() const { return executions; }

        bool loop()
        {
            if (limiter.run())
            {
                task.loop();
                ++executions;
            }

            limiter.delay_remaining();

            return true;
        }

    private:
        BaseTask & task;
        Limiter limiter;
        unsigned int executions = 0;
};
