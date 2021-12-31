/*
    Copyright (C) 2021 Westberry Technology (ChangZhou) Corp., Ltd

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef __CMDINFO_H
#define __CMDINFO_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

#define EX_OK 0
#define EX_USAGE 64
#define EX_DATAERR 65
#define EX_NOINPUT 66
#define EX_SOFTWARE 70
#define EX_CANTCREAT 73
#define EX_IOERR 74
#define EX_PROTOCOL 76

#define CMD_BACK_SUCCESS 0x00
#define CMD_BACK_ARGERR 0x01
#define CMD_BACK_ERASE_FAIL 0x11
#define CMD_BACK_WRITE_FAIL 0x12
#define CMD_BACK_READ_PROTECT 0x1A
#define CMD_BACK_INVALID 0xFF

#define CMD_GET_INFO 0X01
#define CMD_ERASE 0X5E
#define CMD_WRITE_PAGE 0x6A
#define CMD_READ 0x72
#define CMD_RESET 0x83
#define CMD_GO 0x9B

enum mode
{
  MODE_NONE,
  MODE_VERSION,
  MODE_LIST,
  MODE_UPLOAD,
  MODE_DOWNLOAD,
  MODE_RESET,
  MODE_SETADDR
};

#define warx(...) do { \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n"); \
  } while (0)

#define errx(err_id, ...)  do { \
    warx(__VA_ARGS__); \
    exit(err_id); \
  } while (0)

extern void help(void);
extern void print_version(void);

#endif /* __CMDINFO_H */
