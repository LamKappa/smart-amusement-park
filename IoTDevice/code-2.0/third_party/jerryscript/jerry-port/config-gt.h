#ifndef CONFIG_GT_H
#define CONFIG_GT_H

#ifdef JERRY_FOR_IAR_CONFIG

#ifdef JERRY_IAR_GT

#include "mc_fs.h"
#include "mc_memory_config.h"
#include "mc_memory.h"
#include "mc_type.h"
#include "mc_hal_rtc.h"

#define INPUTJS_BUFFER_SIZE (32 * 1024)
#define SNAPSHOT_BUFFER_SIZE (24 * 1024)

// Maximum size for js and snapshot file
__no_init static uint8_t input_buffer[INPUTJS_BUFFER_SIZE] @ ACE_CACHE_ADDRESS;
__no_init static uint8_t snapshot_buffer[SNAPSHOT_BUFFER_SIZE] @ SNAPSHOT_24K_ADDRESS;

#endif // JERRY_IAR_GT
#endif // JERRY_FOR_IAR_CONFIG

#endif // CONFIG_GT_H
