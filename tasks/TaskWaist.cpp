#include "platform.hpp"
#include "platform/config.h"

#include "tasks/TaskWaist.hpp"

#include "MessageAck.hpp"

#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>

#include "posey-platform/platform/io/NordicNUSDriver.h"

#define LOG_MODULE_NAME posey_task_hub
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

bool TaskWaist::setup()
{
    bool success = imu.setup();
    success &= ble.setup();
    success &= ml.add_listener(cmd);
    return success;
}

void TaskWaist::loop()
{
    static const uint32_t max_loop_time = 1e6/50;
    static uint32_t iter = 0;

    tm.message.t_start = Clock::get_usec<uint32_t>();
    tm.message.counter++;

    // // Update sensor data.
    imu.collect();
    ble.collect(); // Does nothing.

    // Serialize data and store to flash.
    imu.serialize();
    process_data(NULL, 255, imu.buffer.get_buffer(), imu.buffer.used());

    BLEData elem;
    while (ble.ring_buffer.read_next(elem))
    {
        elem.serialize(ble.buffer);
        process_data(NULL, 255, ble.buffer.get_buffer(), ble.buffer.used());
    }

    // Check for messages. Process everything available, we'll
    // let this task overrun.
    ml.poll(reader);
    while (true)
    {
        auto mid = ml.process_next();
        if (mid <= -1) break;
        process_message(mid);
    }

    // Send task TM at 1Hz.
    if (iter % 50 == 0)
    {
        tm.serialize();
        writer.write(tm.buffer);
    }
    ++iter;

    // Update missed deadlines if needed.
    tm.message.t_end = Clock::get_usec<uint32_t>();
    if (tm.message.t_end - tm.message.t_start > max_loop_time)
    {
        tm.message.missed_deadline++;
    }
}

void TaskWaist::process_message(const uint16_t mid)
{
    bool invalid_checksum = false;

    // Command message?
    if (mid == Command::message_id)
    {
        // Extract command.
        if (cmd.valid_checksum())
        {
            auto & msg = cmd.deserialize();
            if (msg.command == Command::StartCollecting)
                cmd.message.ack = MessageAck::Working;
            else cmd.message.ack = MessageAck::OK;
            cmd.serialize();
            writer.write(cmd);

            LOG_INF("Received %s command", msg.command_str());

            // Need to stop logging?
            bool need_to_stop_logging =
                (msg.command == Command::Reboot)
                || (msg.command == Command::DownloadData)
                || (msg.command == Command::StopCollecting);
            if (need_to_stop_logging && flash_is_logging())
            {
                stop_flash_logging();
                config_update_data_end_ms(Clock::get_msec<uint32_t>());
            }

            // Handle command.
            switch (msg.command)
            {
                case Command::Reboot:
                    LOG_INF("Waiting 5s then rebooting.");
                    k_sleep(K_SECONDS(5));
                    sys_reboot(SYS_REBOOT_COLD);
                    break;
                
                case Command::GetDataSummary:
                    // Send data summary.
                    memcpy(
                        tm_data_summary.message.datetime,
                        device_config.data_dt,
                        DataSummary::MaxDatetimeSize);
                    tm_data_summary.message.start_ms = device_config.data_start_ms;
                    tm_data_summary.message.end_ms = device_config.data_end_ms;
                    tm_data_summary.message.bytes = device_config.data_end;

                    LOG_INF("Sending summary packet.");
                    tm_data_summary.serialize();
                    writer.write(tm_data_summary);

                    break;
                
                case Command::DownloadData:
                    // Send data.
                    LOG_INF("Initiating data download.");
                    break;
                
                case Command::StartCollecting:
                    // Initiate logging start.
                    cmd.message.payload[DataSummary::MaxDatetimeSize-1] = 0;
                    config_update_data_dt(reinterpret_cast<const char *>(cmd.message.payload));
                    config_update_data_end_ms(Clock::get_msec<uint32_t>());
                    config_update_data_start_ms(Clock::get_msec<uint32_t>());
                    config_update_data_end(0);
                    memcpy(
                        tm_data_summary.message.datetime,
                        device_config.data_dt,
                        DataSummary::MaxDatetimeSize);

                    // Reset FLASH.
                    LOG_INF("Erasing flash, might take a bit...");
                    erase_used_flash();

                    // Start collection.
                    LOG_INF("Starting collection at...");
                    start_flash_logging();

                    // Send ack.
                    cmd.message.ack = MessageAck::OK;
                    cmd.serialize();
                    writer.write(cmd);

                    break;
                
                case Command::StopCollecting:
                    // Don't need to do anything.
                    break;

                case Command::Configure:
                case Command::ConnectPeripheral:
                case Command::NoOp:
                    LOG_WRN("Not implemented.");
                    break;
                default:
                    LOG_WRN("Unknown command");
                    break;
            }
        }
        else
        {
            invalid_checksum = true;
            cmd.message.ack = MessageAck::Resend;
            cmd.serialize();
            writer.write(cmd);
        }
    }

    // Invalid checksum?
    if (invalid_checksum)
    {
        tm.message.invalid_checksum++;
    }
}
