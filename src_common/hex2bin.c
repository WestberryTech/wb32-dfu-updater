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
#include <string.h>

#include "hex2bin.h"
#include "cmdinfo.h"

static uint8_t hex_char_to_bin_char(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'a' && c <= 'z')
    return c - 'a' + 10;
  else if (c >= 'A' && c <= 'Z')
    return c - 'A' + 10;
  return 0xff;
}

static uint8_t hex_to_bin(const char *p)
{
  uint8_t tmp = 0;
  tmp = hex_char_to_bin_char(p[0]);
  tmp <<= 4;
  tmp |= hex_char_to_bin_char(p[1]);
  return tmp;
}

static int parse_line_of_hex(const char *src, bin_farmat_t *p)
{
  uint8_t check = 0, tmp[4], binLen;
  uint16_t hexLen = strlen(src);
  uint16_t num = 0, offset = 0;

  if (hexLen > HEX_MAX_LENGTH)
    return RES_DATA_TOO_LONG;

  if (hexLen < HEX_MIN_LEN)
    return RES_DATA_TOO_SHORT;

  if (src[0] != ':')
    return RES_NO_COLON;

  if ((hexLen - 1) % 2 != 0)
    return RES_LENGTH_ERROR;

  binLen = (hexLen - 1) / 2;

  while (num < 4)
  {
    offset = (num << 1) + 1;
    tmp[num] = hex_to_bin(src + offset);
    check += tmp[num];
    num++;
  }

  p->len = tmp[0];
  p->addr = tmp[1];
  p->addr <<= 8;
  p->addr += tmp[2];
  p->type = tmp[3];

  while (num < binLen)
  {
    offset = (num << 1) + 1;
    p->data[num - 4] = hex_to_bin(src + offset);
    check += p->data[num - 4];
    num++;
  }

  if (p->len != binLen - 5)
    return RES_LENGTH_ERROR;

  if (check != 0)
    return RES_CHECK_ERROR;

  return RES_OK;
}

static void sort_link(parsed_data_t **List)
{
  parsed_data_t *p, *q, *temp_qn, *temp_p;
  parsed_data_t *srcp = *List, *retp = NULL;

  while (srcp != NULL)
  {
    p = srcp;
    q = p->next;
    while (q != NULL)
    {
      if (q->psd_start_addr < p->psd_start_addr)
      {
        temp_p = p;
        temp_qn = q->next;
        p = q;
        q = temp_p;
        p->next = q;
        q->next = temp_qn;
      }
      q = q->next;
    }
    srcp = p;
    if (retp == NULL)
      retp = srcp;
    srcp = srcp->next;
  }

  if (retp != NULL)
    *List = retp;
}

static int parsed_line_data(FILE *hex_file, parsed_data_t **des_block)
{
  parsed_data_t *psd_block;
  uint8_t raw_data[522];
  uint8_t psd_data[255];
  bin_farmat_t fmt_bin;
  result_status_e res;
  uint32_t address;
  uint16_t base_addr = 0;

  *des_block = NULL;
  fmt_bin.data = psd_data;

  while (!feof(hex_file))
  {
    if (fscanf(hex_file, "%s\r\n", raw_data) != 1)
    {
      return RES_HEX_FILE_NO_END;
    }
    res = parse_line_of_hex((const char *)raw_data, &fmt_bin);
    if (res != RES_OK)
      return res;

    switch (fmt_bin.type)
    {
    case 0:
    {
      address = (base_addr << 16) | fmt_bin.addr;
      parsed_data_t *new_block = dfufw_malloc(sizeof(*new_block));
      new_block->psd_data = dfufw_malloc(sizeof(uint8_t) * fmt_bin.len);
      new_block->psd_length = fmt_bin.len;
      new_block->psd_start_addr = address;
      memcpy(new_block->psd_data, (const uint8_t *)fmt_bin.data, new_block->psd_length);
      new_block->next = NULL;
      if (*des_block == NULL)
      {
        *des_block = new_block;
        psd_block = *des_block;
      }
      else
      {
        psd_block->next = new_block;
        psd_block = psd_block->next;
      }
    }
    break;
    case 1:
      return RES_OK;
      break;
    case 4:
      base_addr = (fmt_bin.data[0] << 8) | fmt_bin.data[1];
      break;
    case 5:
      break;
    default:
      return RES_TYPE_ERROR;
      break;
    }
  }

  return RES_HEX_FILE_NO_END;
}

static int get_block_size(parsed_data_t *src_blcok)
{
  uint32_t block_size = 0;
  uint32_t last_addr = src_blcok->psd_start_addr;
  uint32_t last_len = 0;

  for (; src_blcok != NULL; src_blcok = src_blcok->next)
  {
    uint32_t addr = src_blcok->psd_start_addr;
    uint32_t len = src_blcok->psd_length;
    if (addr > (last_addr + last_len))
    {
      return block_size;
    }
    block_size += len;
    last_addr = addr;
    last_len = len;
  }
  if (src_blcok == NULL)
    return block_size;

  return -1;
}

static int parsed_continuous_data(parsed_data_t *psd_vld, parsed_data_t **des_block)
{
  int ret;
  uint32_t off_set;
  parsed_data_t *tmp_save;

  if (psd_vld)
  {
    while (psd_vld)
    {
      ret = get_block_size(psd_vld);
      if (ret > 0)
      {
        off_set = 0;
        parsed_data_t *vld_block = dfufw_malloc(sizeof(*vld_block));
        vld_block->psd_data = dfufw_malloc(sizeof(uint8_t) * ret);
        vld_block->psd_length = ret;
        vld_block->psd_start_addr = psd_vld->psd_start_addr;
        vld_block->next = NULL;

        while (psd_vld && (off_set < vld_block->psd_length))
        {
          memcpy(&vld_block->psd_data[off_set], psd_vld->psd_data, psd_vld->psd_length);
          off_set += psd_vld->psd_length;
          psd_vld = psd_vld->next;
        }
        if (*des_block == NULL)
        {
          *des_block = vld_block;
          tmp_save = *des_block;
        }
        else
        {
          tmp_save->next = vld_block;
          tmp_save = tmp_save->next;
        }
      }
      else
      {
        return RES_LENGTH_ERROR;
      }
    }
  }

  return RES_OK;
}

static void free_temp_mem(parsed_data_t **src)
{
  parsed_data_t *temp;
  parsed_data_t *prev = NULL;

  for (temp = *src; temp != NULL; temp = temp->next)
  {
    free(prev);
    prev = temp;
  }
  free(prev);
  *src = NULL;
}

int parsed_hex_file(dw_flasher_t *flasher)
{
  int ret;
  FILE *hex_file;
  parsed_data_t *psd_bin = NULL;

  hex_file = fopen(flasher->file_name, "r");
  if (!hex_file)
    return RES_BIN_FILE_PATH_ERROR;

  printf("----------------------------------------\n");
  printf("%s file opened\n", flasher->file_name);

  fseek(hex_file, 0, SEEK_SET);

  ret = parsed_line_data(hex_file, &psd_bin);
  fclose(hex_file);

  if (ret == RES_OK)
  {
    sort_link(&psd_bin);
    free(flasher->firmware);
    ret = parsed_continuous_data(psd_bin, &flasher->firmware);
    free_temp_mem(&psd_bin);
  }

  return ret;
}
