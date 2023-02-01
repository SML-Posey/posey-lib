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
} DeviceConfig;
extern DeviceConfig device_config;

#ifdef __cplusplus
}
#endif