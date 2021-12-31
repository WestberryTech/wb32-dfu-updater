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
