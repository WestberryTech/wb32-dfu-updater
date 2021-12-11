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
