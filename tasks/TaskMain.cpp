#include "platform.hpp"

#include "tasks/TaskMain.hpp"

#include "MessageAck.hpp"

bool TaskMain::setup()
{
    bool success = sensors.setup();
    success &= motor.setup();
    success &= ml.add_listener(cmd);
    return success;
}

void TaskMain::loop()
{
    static uint16_t iter = 0;
    ++iter;

    tm.message.t_start = Clock::get_usec<uint32_t>();

    // Update sensor data.
    sensors.collect();

    // Do stuff.
    pwm.cycle(iter);

    // Check for messages. Process everything available, we'll
    // let this task overrun.
    ml.poll(reader);
    while (true)
    {
        auto mid = ml.process_next();
        if (mid <= -1) break;
        process_message(mid);
    }

    // Send sensor telemetry.
    sensors.adc.write_telemetry(writer);
    sensors.temp.write_telemetry(writer);

    // Send task TM.
    tm.message.pwm_dc = pwm.get_duty_cycle();
    tm.message.t_end = Clock::get_usec<uint32_t>();
    tm.serialize();
    writer.write(tm.buffer);
}

void TaskMain::process_message(const uint16_t mid)
{
    bool invalid_checksum = false;

    // Command message?
    if (mid == Command::message_id)
    {
        // Extract command.
        if (cmd.valid_checksum())
        {
            auto & msg = cmd.deserialize();

            switch (msg.command)
            {
                case Command::MotorStop:
                    // Stop motor.
                    motor.set_torque(0);
                    break;

                case Command::MotorPosition:
                {
                    // Set motor position.
                    float position = msg.arg1;
                    motor.set_position(position);
                    break;
                }

                case Command::MotorTorque:
                {
                    // Set motor torque.
                    float torque = msg.arg2;
                    motor.set_torque(torque);
                    break;
                }

                case Command::PowerStop:
                    // Stop PWMs.
                    pwm.stop();
                    break;

                case Command::PowerPWM:
                {
                    // Set PWM.
                    float dc = msg.arg1;
                    float cycle_period = msg.arg2;
                    uint16_t cycle_mod = cycle_period*25;
                    pwm.set_cycle_mod(cycle_mod);
                    pwm.set_duty_cycle(dc);
                    break;
                }

                case Command::NoOp:
                default:
                    break;
            }

            cmd.message.ack = MessageAck::OK;
        }
        else
        {
            invalid_checksum = true;
            cmd.message.ack = MessageAck::Resend;
        }

        // Send acknowledgement.
        cmd.serialize();
        writer.write(cmd);
    }

    // Invalid checksum?
    if (invalid_checksum)
    {
        tm.message.invalid_checksum++;
    }
}
