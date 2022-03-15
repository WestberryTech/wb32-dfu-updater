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

#ifndef __CONFIG_H
#define __CONFIG_H

#include "cmake_config.h"

#define WB32_DFU_UPDATER_VERSION PROJECT_VERSION

#define MAX_DESC_STR_LEN 255
#define FLASH_PAGE_SIZE 256
#define DEFAULT_START_ADDR 0x8000000

#define INFO4_ADDR 0x1FFFF400
#define INFO5_ADDR 0x1FFFF500
#define INFO6_ADDR 0x1FFFF600

#define UNREAD_PROT_TIMEOUT 30 /* s */

#define TRANSFER_SIZE_LIMIT 1024
#define READ_DATA_SIZE TRANSFER_SIZE_LIMIT

#define DEFAULT_TIMEOUT 5000 /* seconds */

#endif /* __CONFIG_H */
