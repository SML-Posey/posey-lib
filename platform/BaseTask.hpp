#pragma once

class BaseTask
{
    public:
        virtual ~BaseTask() {}

        virtual bool setup() = 0;
        virtual void loop() = 0;
};
