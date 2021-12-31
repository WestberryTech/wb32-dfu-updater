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

#ifndef HEX2BIN_H
#define HEX2BIN_H

#include <stdint.h>

#include "dfufw.h"

#define HEX_MAX_LENGTH 521
#define HEX_MIN_LEN 11

typedef enum
{
  RES_OK = 0,
  RES_DATA_TOO_LONG,
  RES_DATA_TOO_SHORT,
  RES_NO_COLON,
  RES_TYPE_ERROR,
  RES_LENGTH_ERROR,
  RES_CHECK_ERROR,
  RES_HEX_FILE_NOT_EXIST,
  RES_BIN_FILE_PATH_ERROR,
  RES_WRITE_ERROR,
  RES_HEX_FILE_NO_END
} result_status_e;

typedef struct
{
  uint8_t len;
  uint8_t type;
  uint16_t addr;
  uint8_t *data;
} bin_farmat_t;

int parsed_hex_file(dw_flasher_t *flasher);
result_status_e hex_file_to_bin_file(dw_flasher_t *flasher, char *dest);
#endif
