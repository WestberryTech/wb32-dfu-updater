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

#include <stdio.h>

#include "libusb.h"
#include "config.h"
#include "cmdinfo.h"

void help(void)
{
  fprintf(stderr, "Usage: wb32-dfu-updater [options] ...\n"
                  "  -h --help\t\t\tPrint this help message\n"
                  "  -V --version\t\t\tPrint the version number\n"
                  "  -l --list\t\t\tList currently attached DFU capable devices\n");
  fprintf(stderr, "  -n --devnum <dnum>\t\tMatch given device number (devnum from --list)\n");
  fprintf(stderr, "  -R --reset\t\t\tControl device reset\n"
                  "  -s --dfuse-address <address> \tSet the start address for downloading firmware\n"
                  "  -U --upload <file>\t\tRead firmware from device into <file>\n"
                  "  -Z --upload-size <bytes>\tSpecify the expected upload size in bytes\n"
                  "  -D --download <file>\t\tWrite firmware from <file> into device\n"
                  "  -w --wait\t\t\tWait for device to appear\n");
}

void print_version(void)
{
  printf("wb32-dfu-updater ver: " WB32_DFU_UPDATER_VERSION "\n");

#if defined(LIBUSB_API_VERSION) || defined(LIBUSBX_API_VERSION)
  const struct libusb_version *ver;
  ver = libusb_get_version();
  printf("libusb version %i.%i.%i%s (%i)\n", ver->major,
         ver->minor, ver->micro, ver->rc, ver->nano);
  printf("----------------------------------------\n");
#else
  warnx("libusb version is ancient");
#endif
}
