#ifndef APP_ACCESS_H_
#define APP_ACCESS_H_

#include <stdint.h>

extern const uint8_t REG_ACCESS[];

#define USER_WRITE_ACCESS	0x20
#define CONFIG_WRITE_ACCESS	0x10

#define USER_READ_ACCESS	0x02
#define CONFIG_READ_ACCESS	0x01

#endif
