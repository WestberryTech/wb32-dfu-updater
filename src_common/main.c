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

#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>

#if !(defined(_WIN32) || defined(__CYGWIN__))
#    include <unistd.h>
#else
#    include <io.h>
#endif

#include "dfufwdevice.h"
#include "cmdinfo.h"
#include "libusb.h"
#include "dfufw.h"

dfu_dev_t *dfu_root = NULL;
uint8_t toolbox_mode = 0;

static long parse_dwaddr(char *str, char *nmb)
{
  char *comma = NULL;
  char *endptr;
  long adrval = 0;

  comma = strstr(nmb, "0x");

  if (comma == NULL)
    comma = strstr(nmb, "0X");

  if (comma != NULL)
    comma += 2;
  else
    comma = nmb;

  errno = 0;
  adrval = strtol(comma, &endptr, 16);

  if ((errno == ERANGE && (adrval == LONG_MAX || adrval == LONG_MIN)) ||
      (errno != 0 && adrval == 0) || (*endptr != '\0'))
  {
    errx(EX_USAGE, "Something went wrong with the argument of --%s\n", str);
  }

  if (endptr == comma)
  {
    errx(EX_USAGE, "No digits were found from the argument of --%s\n", str);
  }

  return adrval;
}

static int parse_number(char *str, char *nmb)
{
  char *comma = NULL;
  char *endptr;
  char ishex = 0;
  long val;

  comma = strstr(nmb, "0x");

  if (comma == NULL)
    comma = strstr(nmb, "0X");

  if (comma != NULL)
  {
    comma += 2;
    ishex = 1;
  }
  else
  {
    comma = nmb;
  }

  errno = 0;

  if (ishex)
  {
    val = strtol(comma, &endptr, 16);
  }
  else
  {
    val = strtol(comma, &endptr, 0);
  }

  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) ||
      (errno != 0 && val == 0) || (*endptr != '\0'))
  {
    fprintf(stderr, "Something went wrong with the argument of --%s\n", str);
    exit(EX_USAGE);
  }

  if (endptr == comma)
  {
    fprintf(stderr, "Something went wrong with the argument of --%s\n", str);
    exit(EX_USAGE);
  }

  return (int)val;
}

static struct option options[] = {
    {"help", 0, 0, 'h'},
    {"version", 0, 0, 'V'},
    {"list", 0, 0, 'l'},
    {"devnum", 1, 0, 'n'},
    {"reset", 0, 0, 'R'},
    {"dfuse-address", 1, 0, 's'},
    {"upload", 1, 0, 'U'},
    {"upload-size", 1, 0, 'Z'},
    {"download", 1, 0, 'D'},
    {"toolbox-mode", 0, 0, 't'},
    {"wait", 1, 0, 'w'},
    {0, 0, 0, 0}};

int main(int argc, char *argv[])
{
  uint32_t dw_addr = 0;
  int expected_size = 0;
  uint16_t match_dev_num = 0;
  enum mode mode = MODE_NONE;
  libusb_context *ctx;
  dw_flasher_t flasher = {.dw_addr = DEFAULT_START_ADDR};
  int wait_device = 0;
  int ret;

  setvbuf(stdout, NULL, _IONBF, 0);

  while (1)
  {
    int parc, ioption = 0;

    parc = getopt_long(argc, argv, "hVln:s:RU:D:Z:tw", options, &ioption);
    if (parc == -1)
      break;

    switch (parc)
    {
    case 'h':
      help();
      exit(EX_OK);
      break;
    case 'V':
      mode = MODE_VERSION;
      break;
    case 'l':
      mode = MODE_LIST;
      break;
    case 'n':
      match_dev_num = parse_number("devnum", optarg);
      break;
    case 's':
      dw_addr = parse_dwaddr("dfuse-address", optarg);
      if (dw_addr < 0x8000000)
      {
        errx(EX_USAGE, "The set address is not supported. "
             "Address range 0x8000000 - (0x8000000 + chip flash size)");
      }
      flasher.dw_addr = dw_addr;
      break;
    case 'R':
      mode = MODE_RESET;
      break;
    case 'D':
      mode = MODE_DOWNLOAD;
      flasher.file_name = optarg;
      break;
    case 'U':
      mode = MODE_UPLOAD;
      flasher.file_name = optarg;
      break;
    case 'Z':
      expected_size = parse_number("upload-size", optarg);
      break;
    case 't':
      toolbox_mode = 1;
      break;
    case 'w':
      wait_device = 1;
      break;
    default:
      help();
      exit(EX_USAGE);
      break;
    }
  }

  if (optind != argc)
  {
    fprintf(stderr, "Error: Unexpected argument: %s\n\n", argv[optind]);
    help();
    exit(EX_USAGE);
  }

  if (mode == MODE_VERSION)
  {
    print_version();
    exit(EX_OK);
  }

  if (mode == MODE_NONE)
  {
    fprintf(stderr, "You need to specify one of -D or -U\n");
    help();
    exit(EX_USAGE);
  }

  if (mode == MODE_DOWNLOAD)
  {
    dfu_load_file(&flasher);
  }

  ret = libusb_init(&ctx);
  if (ret)
  {
    errx(EX_IOERR, "unable to initialize libusb: %s", libusb_error_name(ret));
  }

probe:
  probe_devices(ctx);

  if (mode == MODE_LIST)
  {
    list_dfu_interfaces();
    disconnect_devices();
    libusb_exit(ctx);
    return EX_OK;
  }

  if (dfu_root == NULL)
  {
    if (wait_device)
    {
      if (!ret)
      {
        printf("Waiting for device, exit with ctrl-C\n");
        ret = -1;
      }
      milli_sleep(500);
      printf(".");
      goto probe;
    }
    else
    {
      warx("No DFU capable USB device available");
      libusb_exit(ctx);
      return EX_IOERR;
    }
  }
  else if (dfu_root->next != NULL)
  {
    dfu_dev_t *pdfu;

    if (match_dev_num == 0)
      errx(EX_IOERR, "More than one DFU capable USB device found! "
                     "Try `--list' and specify the device number "
                     "or disconnect all but one device");
    for (pdfu = dfu_root; pdfu != NULL; pdfu = pdfu->next)
    {
      if (pdfu->devnum == match_dev_num)
        break;
    }
    if (pdfu == NULL)
    {
      errx(EX_IOERR, "The device whose devnu is %d was not found!", match_dev_num);
    }
    dfu_root = pdfu;
  }

  printf("----------------------------------------\n");
  printf("Found DFU\n");

  if (ret)
    printf("\n");
  printf("Opening DFU capable USB device ...\n");

  ret = libusb_open(dfu_root->dev, &dfu_root->dev_handle);
  if (ret || !dfu_root->dev_handle)
    errx(EX_IOERR, "Cannot open device: %s", libusb_error_name(ret));
  printf("Device ID %04x:%04x\n", dfu_root->vendor, dfu_root->product);

  if (libusb_kernel_driver_active(dfu_root->dev_handle, 0) == 1)
  {
    if (libusb_detach_kernel_driver(dfu_root->dev_handle, 0))
    {
      libusb_free_device_list(&dfu_root->dev, 1);
      libusb_close(dfu_root->dev_handle);
      errx(EX_SOFTWARE, "Couldn't detach kernel driver!");
    }
  }

  ret = libusb_claim_interface(dfu_root->dev_handle, 0);
  if (ret < 0)
  {
    libusb_free_device_list(&dfu_root->dev, 1);
    libusb_close(dfu_root->dev_handle);
    errx(EX_SOFTWARE, "Cannot Claim Interface");
  }

  switch (mode)
  {
  case MODE_DOWNLOAD:
    dfufw_opt_download(dfu_root, &flasher);
    break;
  case MODE_UPLOAD:
    dfufw_opt_upload(dfu_root, &flasher, expected_size);
    break;
  case MODE_RESET:
    dfufw_opt_reset(dfu_root);
    break;
  default:
    exit(EX_USAGE);
    break;
  }

  libusb_close(dfu_root->dev_handle);
  dfu_root->dev_handle = NULL;
  exit(EX_OK);

  return 0;
}