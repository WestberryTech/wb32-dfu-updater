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

#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

#if !(defined(_WIN32) || defined(__CYGWIN__))
#  include <unistd.h>
#else
#  include <io.h>
#endif

#include "dfufw.h"
#include "cmdinfo.h"
#include "hex2bin.h"
#include "dfufwdevice.h"

void *dfufw_malloc(size_t size)
{
  void *ptr = malloc(size);
  if (ptr == NULL)
  {
    errx(EX_SOFTWARE, "Cannot allocate memory of size %d bytes", (int)size);
  }
  return (ptr);
}

const char *TipString[] = {
  "bin file to hex file success!",
  "line data of hex file is too large!",
  "line data of hex file is too short!",
  "line data of hex file is no colon!",
  "line data of hex file type is error!",
  "line data of hex file length is error!",
  "line data of hex file check error!",
  "hex file is not exist!",
  "bin file path is error!",
  "hex file is no end!"};

void dfu_load_file(dw_flasher_t *flasher)
{
  result_status_e res;
  long int offset;
  int f;

  if (strstr(flasher->file_name, ".hex") != NULL)
  {
    res = parsed_hex_file(flasher);
    if (res != RES_OK)
    {
      errx(EX_USAGE, "Error parsing hex file, %s", TipString[res]);
    }
  }
  else if (strstr(flasher->file_name, ".bin") != NULL)
  {
    ssize_t read_count;
    long int read_total = 0;
    flasher->firmware = dfufw_malloc(sizeof(*flasher->firmware));

    f = open(flasher->file_name, O_RDONLY | O_BINARY);
    if (f < 0)
    {
      errx(EX_NOINPUT, "Could not open file %s for reading\n", flasher->file_name);
    }

    printf("----------------------------------------\n");
    printf("%s file opened\n", flasher->file_name);

    offset = lseek(f, 0, SEEK_END);

    if (offset < 0)
    {
      fprintf(stderr, "File size is too big\n");
      exit(EX_SOFTWARE);
    }

    if (lseek(f, 0, SEEK_SET) != 0)
    {
      fprintf(stderr, "Could not seek to beginning\n");
      exit(EX_IOERR);
    }

    flasher->firmware->psd_length = offset;

    if (flasher->firmware->psd_length > (uint32_t)0XFFFFFFFF)
    {
      fprintf(stderr, "File too large for memory allocation on this platform\n");
      exit(EX_SOFTWARE);
    }

    flasher->firmware->next = NULL;
    flasher->firmware->psd_data = dfufw_malloc(sizeof(uint8_t) *
                                               flasher->firmware->psd_length);
    flasher->firmware->psd_start_addr = flasher->dw_addr;

    while (read_total < flasher->firmware->psd_length)
    {
      long int to_read = flasher->firmware->psd_length - read_total;
      /* read() limit on Linux, slightly below MAX_INT on Windows */
      if (to_read > 0x7ffff000)
        to_read = 0x7ffff000;
      read_count = read(f, flasher->firmware->psd_data + read_total, to_read);
      if (read_count == 0)
        break;
      if (read_count == -1 && errno != EINTR)
        break;
      read_total += read_count;
    }
    if (read_total != flasher->firmware->psd_length)
    {
      fprintf(stderr, "Could only read %lld of %lld bytes from %s\n",
              (long long)read_total, (long long)flasher->firmware->psd_length, flasher->file_name);
      exit(EX_IOERR);
    }
    close(f);
  }
  else
  {
    errx(EX_USAGE, "The file isn't bin file, no support!");
  }
}

static void probe_configuration(libusb_device *dev, struct libusb_device_descriptor *desc)
{
  libusb_device_handle *devh;
  struct libusb_config_descriptor *cfg;
  const struct libusb_interface_descriptor *intf;
  const struct libusb_interface *uif;
  dfu_dev_t *pdfu;
  unsigned char manufacturer_name[MAX_DESC_STR_LEN + 1];
  unsigned char product_name[MAX_DESC_STR_LEN + 1];
  int ret;

  do
  {
    if (desc->bNumConfigurations != 0x01)
      return;
    ret = libusb_get_config_descriptor(dev, 0, &cfg);
    if ((ret != 0) || (!cfg))
      return;

    uif = &cfg->interface[0];
    if (!uif)
      break;
    intf = &uif->altsetting[0];
    if (!intf)
      break;

    if ((cfg->bNumInterfaces != 0x01) ||
        (intf->bInterfaceClass != 0xff) ||
        (intf->bInterfaceSubClass != 0xff) ||
        (intf->bNumEndpoints != 0x02) ||
        (!(intf->endpoint[0].bEndpointAddress & 0x80)) ||
        ((intf->endpoint[1].bEndpointAddress & 0x80)))
      break;
    else
    {
      ret = libusb_open(dev, &devh);
      if (ret)
      {
        printf("open usb fail\n");
        fprintf(stderr, "Cannot open DFU device %04x:%04x found on devnum %i (%s)",
                desc->idVendor, desc->idProduct, libusb_get_device_address(dev),
                libusb_error_name(ret));
        fprintf(stderr, "\n");
        break;
      }
      ret = libusb_get_string_descriptor_ascii(devh, 0x01, manufacturer_name,
                                               MAX_DESC_STR_LEN);
      if (ret > 0)
      {
        ret = -1;
        if (strcmp((const char *)manufacturer_name, "Westberry Tech.") == 0)
        {
          ret = libusb_get_string_descriptor_ascii(devh, 0x02, product_name,
                                                   MAX_DESC_STR_LEN);
          if (ret > 0)
          {
            if (strcmp((const char *)product_name, "WB Device in DFU Mode") != 0)
              ret = -1;
          }
          else
          {
            warx("Failed to get product name: %s", libusb_error_name(ret));
          }
        }
      }
      else
      {
        warx("Failed to get manufacturer name: %s", libusb_error_name(ret));
      }

      libusb_close(devh);

      if (ret >= 0)
      {
        pdfu = dfufw_malloc(sizeof(*pdfu));
        memset(pdfu, 0, sizeof(*pdfu));

        pdfu->dev = libusb_ref_device(dev);
        pdfu->vendor = desc->idVendor;
        pdfu->product = desc->idProduct;
        pdfu->bcdDevice = desc->bcdDevice;
        pdfu->configuration = cfg->bConfigurationValue;
        pdfu->interface = intf->bInterfaceNumber;
        pdfu->devnum = libusb_get_device_address(dev);
        pdfu->busnum = libusb_get_bus_number(dev);
        pdfu->bMaxPacketSize0 = desc->bMaxPacketSize0;

        /* queue into list */
        pdfu->next = dfu_root;
        dfu_root = pdfu;
      }
    }
  } while (0);

  libusb_free_config_descriptor(cfg);
}

void probe_devices(libusb_context *ctx)
{
  libusb_device **list;
  ssize_t num_devs;
  ssize_t i;

  num_devs = libusb_get_device_list(ctx, &list);
  for (i = 0; i < num_devs; ++i)
  {
    struct libusb_device_descriptor desc;
    struct libusb_device *dev = list[i];

    if (libusb_get_device_descriptor(dev, &desc))
      continue;
    if ((desc.idVendor == 0x342D) && (desc.idProduct == 0xDFA0))
      probe_configuration(dev, &desc);
  }
  libusb_free_device_list(list, 1);
}

void disconnect_devices(void)
{
  dfu_dev_t *pdfu;
  dfu_dev_t *prev = NULL;

  for (pdfu = dfu_root; pdfu != NULL; pdfu = pdfu->next)
  {
    free(prev);
    libusb_unref_device(pdfu->dev);
    prev = pdfu;
  }
  free(prev);
  dfu_root = NULL;
}

void print_dfu_if(dfu_dev_t *dfu_if)
{
  printf("Found DFU: [0x%04X:0x%04X] ver=0x%04x, devnum=%u, cfg=%u, intf=%u \n",
         dfu_if->vendor, dfu_if->product,
         dfu_if->bcdDevice, dfu_if->devnum,
         dfu_if->configuration, dfu_if->interface);
}

/* Walk the device tree and print out DFU devices */
void list_dfu_interfaces(void)
{
  dfu_dev_t *pdfu = dfu_root;

  if (pdfu == NULL)
    warx("Not found device!");

  for (; pdfu != NULL; pdfu = pdfu->next)
    print_dfu_if(pdfu);
}

void write_fm_to_flash(dfu_dev_t *pdfu, uint32_t addr, uint8_t *fm_data, uint32_t total)
{
  uint8_t page_buf[FLASH_PAGE_SIZE] = {0};
  uint8_t vrf_buf[FLASH_PAGE_SIZE] = {0};
  uint32_t page_addr;
  uint32_t remain;
  uint32_t size = total;

  remain = addr % FLASH_PAGE_SIZE;
  page_addr = addr - remain;

  while (size >= FLASH_PAGE_SIZE)
  {
    fn_read_data(pdfu, page_addr, page_buf, FLASH_PAGE_SIZE);
    fn_erase_pages(pdfu, page_addr, 1);
    memcpy(&page_buf[remain], &fm_data[total - size], FLASH_PAGE_SIZE - remain);
    fn_program_page(pdfu, page_addr, page_buf, 0, FLASH_PAGE_SIZE);
    fn_read_data(pdfu, page_addr, vrf_buf, FLASH_PAGE_SIZE);
    if (memcmp(page_buf, vrf_buf, FLASH_PAGE_SIZE))
      errx(EX_USAGE, "Check Failed in page address 0x%8X", page_addr);
    size -= (FLASH_PAGE_SIZE - remain);
    page_addr += FLASH_PAGE_SIZE;
    if (remain)
      remain = 0;
  }
  if (size < FLASH_PAGE_SIZE)
  {
    fn_read_data(pdfu, page_addr, page_buf, FLASH_PAGE_SIZE);
    fn_erase_pages(pdfu, page_addr, 1);
    memcpy(&page_buf[remain], &fm_data[total - size], size);
    fn_program_page(pdfu, page_addr, page_buf, 0, FLASH_PAGE_SIZE);
    fn_read_data(pdfu, page_addr, vrf_buf, FLASH_PAGE_SIZE);
    if (memcmp(page_buf, vrf_buf, FLASH_PAGE_SIZE))
      errx(EX_USAGE, "Check Failed in page address 0x%8X", page_addr);
    size -= size;
    page_addr += FLASH_PAGE_SIZE;
    if (remain)
      remain = 0;
  }
}

int dfufw_opt_download(dfu_dev_t *pdfu, dw_flasher_t *flasher)
{
  parsed_data_t *dw_file;
  uint8_t chip_info[13];

  fn_get_info(pdfu, 0x00, chip_info);
  uint8_t usb_bt_ver = chip_info[0];
  uint32_t chip_id = *((uint32_t *)&chip_info[1]);
  uint32_t flash_size = *((uint32_t *)&chip_info[5]);
  uint32_t sram_size = *((uint32_t *)&chip_info[9]);

  printf("----------------------------------------\n");
  printf("The device bootloader version: %d.%d\n", usb_bt_ver >> 4, usb_bt_ver & 0x0f);
  printf("Chip id: 0x%08X\n", chip_id);
  printf("Flash size: %d KBytes\n", flash_size >> 10);
  printf("Sram size: %d KBytes\n", sram_size >> 10);
  printf("----------------------------------------\n");

  if ((chip_id & 0x3FFF) != 0x2980)
    errx(EX_USAGE,
         "Unknown Chip (0x%08X), Maybe use the latest software to solve the problem",
         chip_id);

  printf("Start Download ...\n");

  for (dw_file = flasher->firmware; dw_file != NULL; dw_file = dw_file->next)
  {
    if ((dw_file->psd_start_addr + dw_file->psd_length) > (0x8000000 + flash_size))
    {
      errx(EX_USAGE,
           "The content out of flash address range. \
           The flash address range is 0x08000000-0x%08X}",
           0x08000000 + flash_size - 1);
    }
    printf("Download block start address: 0x%08X\n", dw_file->psd_start_addr);
    printf("Download block size: %d Bytes\n", dw_file->psd_length);
    printf("Writing ...\n");
    write_fm_to_flash(pdfu, dw_file->psd_start_addr, dw_file->psd_data, dw_file->psd_length);
  }

  printf("OK\n");
  printf("Download completed!\n");

  return 0;
}

int dfufw_opt_upload(dfu_dev_t *pdfu, dw_flasher_t *flasher, uint32_t size)
{
  FILE *f;
  uint8_t *read_buf = dfufw_malloc(sizeof(uint8_t) * size);
  f = fopen(flasher->file_name, "wb");
  if (f == NULL)
  {
    errx(EX_NOINPUT, "Could not open file %s for writing\n", flasher->file_name);
  }
  fn_read_data(pdfu, 0x8000000, read_buf, size);
  fwrite(read_buf, 1, size, f);
  free(read_buf);
  fclose(f);
  printf("Upload successed!\n");

  return 0;
}

void dfufw_opt_reset(dfu_dev_t *pdfu)
{
  fn_reset(pdfu, 1000);
  printf("Reset device completed!\n");
}
