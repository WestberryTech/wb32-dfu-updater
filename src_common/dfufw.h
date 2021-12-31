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

#ifndef __DFUFW_H
#define __DFUFW_H

#include <time.h>
#include <stdint.h>
#include "libusb.h"

#include "config.h"

#if !(defined(_WIN32) || defined(__CYGWIN__))
# ifndef O_BINARY
#   define O_BINARY 0
# endif
#endif

#if !(defined(_WIN32) || defined(__CYGWIN__))
#  define milli_sleep(msec) do { \
    if (msec != 0) { \
      struct timespec nanosleepDelay = {(msec) / 1000, ((msec) % 1000) * 1000000}; \
      nanosleep(&nanosleepDelay, NULL); \
    } } while (0)
#else
#  define milli_sleep(msec) do {\
  if (msec != 0) {\
    Sleep(msec);\
  } } while (0)
#endif

typedef struct parsed_data
{
  uint32_t psd_length;
  uint32_t psd_start_addr;
  uint8_t *psd_data;
  struct parsed_data *next;
} parsed_data_t;

typedef struct
{
  uint32_t dw_addr;
  char *file_name;
  parsed_data_t *firmware;
} dw_flasher_t;

typedef struct dfu_dev
{
  uint16_t busnum;
  uint16_t devnum;
  uint16_t vendor;
  uint16_t product;
  uint16_t bcdDevice;
  uint8_t configuration;
  uint8_t interface;
  uint8_t bMaxPacketSize0;
  libusb_device *dev;
  libusb_device_handle *dev_handle;
  struct dfu_dev *next;
} dfu_dev_t;
extern dfu_dev_t *dfu_root;

void disconnect_devices(void);
void list_dfu_interfaces(void);
void *dfufw_malloc(size_t size);
void dfufw_opt_reset(dfu_dev_t *pdfu);
void probe_devices(libusb_context *ctx);
void dfu_load_file(dw_flasher_t *flasher);
int dfufw_opt_download(dfu_dev_t *pdfu, dw_flasher_t *flasher);
int dfufw_opt_upload(dfu_dev_t *pdfu, dw_flasher_t *flasher, uint32_t size);

#endif /* __DFUFW_H */
