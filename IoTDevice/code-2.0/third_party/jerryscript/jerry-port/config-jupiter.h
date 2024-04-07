#ifndef CONFIG_JUPITER_H
#define CONFIG_JUPITER_H

#ifdef JERRY_FOR_IAR_CONFIG

#ifdef JERRY_IAR_JUPITER

#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include "dirent.h"
#include "ohos_types.h"

// Maximum size for js and snapshot file
#define INPUTJS_BUFFER_SIZE (32 * 1024)
#define SNAPSHOT_BUFFER_SIZE (24 * 1024)
#define CONVERTION_RATIO (1024)

#define GT_TASK_HEAP_SIZE (48)
#define BMS_TASK_FOR_CONTEXT_SIZE (3)
#define BMS_TASK_CONTEXT_AND_HEAP_SIZE (GT_TASK_HEAP_SIZE + BMS_TASK_FOR_CONTEXT_SIZE)

#define JS_TASK_CONTEXT_AND_HEAP_SIZE_BYTE (51416)  // >= 51400 + 8

#endif // JERRY_IAR_JUPITER
#endif // JERRY_FOR_IAR_CONFIG

#endif // CONFIG_JUPITER_H
