#ifndef __DFUFWDEVICE_H
#define __DFUFWDEVICE_H

#include "libusb.h"
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "dfufw.h"

void fn_erase_chip(dfu_dev_t *pdfu);
void fn_reset(dfu_dev_t *pdfu, uint32_t delay_ms);
int  fn_get_info(dfu_dev_t *pdfu, int8_t info_id, uint8_t *info);
void fn_go(dfu_dev_t *pdfu, uint32_t address, uint32_t delay_ms);
void fn_erase_pages(dfu_dev_t *pdfu, uint32_t page_addr, uint32_t num_pages);
int  fn_read_data(dfu_dev_t *pdfu, uint32_t address, uint8_t *buffer, uint32_t count);
void fn_cmd_read(dfu_dev_t *pdfu, uint32_t addr, uint8_t *buffer, uint32_t offset, uint32_t count);
void fn_program_page(dfu_dev_t *pdfu, uint32_t page_addr, uint8_t *data, uint32_t offset, uint32_t count);

#endif /* __DFUFWDEVICE_H */
