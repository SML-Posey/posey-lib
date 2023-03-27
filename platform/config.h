#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define DeviceConfigBufferSize 30
typedef struct {
    char name[DeviceConfigBufferSize];
    char role[DeviceConfigBufferSize];
    char hw[DeviceConfigBufferSize];
    char sw[DeviceConfigBufferSize];
    char dt[DeviceConfigBufferSize];

    char data_dt[DeviceConfigBufferSize];
    uint32_t data_start_ms;
    uint32_t data_end_ms;
    uint32_t data_end;
} DeviceConfig;
extern DeviceConfig device_config;

void refresh_device_config();
void config_update_data_dt(const char * dt);
void config_update_data_start_ms(const uint32_t offset);
void config_update_data_end_ms(const uint32_t offset);
void config_update_data_end(const uint32_t offset);

#ifdef __cplusplus
}
#endif