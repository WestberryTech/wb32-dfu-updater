#include "dfufwdevice.h"
#include "cmdinfo.h"

static int gs_dfufw_timeout = DEFAULT_TIMEOUT;

static int fn_en_usb_write(dfu_dev_t *pdfu, uint8_t *data, uint32_t length)
{
  int ret;
  int transferred = -1;

  if (data == NULL && length)
    return -1;

  ret = libusb_bulk_transfer(pdfu->dev_handle,
                             0x01,
                             data,
                             length,
                             &transferred,
                             gs_dfufw_timeout);

  if (ret < 0)
    errx(EX_IOERR, "The write transmission error: %s", libusb_error_name(ret));

  return transferred;
}

static int fn_en_usb_read(dfu_dev_t *pdfu, uint8_t *data, uint32_t length)
{
  int ret;
  int transferred = -1;

  if (data == NULL)
    return -1;

  ret = libusb_bulk_transfer(pdfu->dev_handle,
                             0x81,
                             data,
                             length,
                             &transferred,
                             gs_dfufw_timeout);

  if (ret < 0)
    errx(EX_IOERR, "The read transmission error: %s", libusb_error_name(ret));

  return transferred;
}

static int command_transfer(dfu_dev_t *pdfu,
                            uint8_t *cmd,
                            uint32_t cmd_len,
                            uint8_t *ret_data,
                            uint32_t ret_len)
{
  int ret;

  if (cmd == NULL || ret_data == NULL)
    return -1;

  ret = fn_en_usb_write(pdfu, cmd, cmd_len);

  if (ret != cmd_len)
    return -1;

  if (cmd_len % pdfu->bMaxPacketSize0 == 0)
  {
    ret = fn_en_usb_write(pdfu, NULL, 0);
    if (ret != 0)
      return -1;
  }

  ret = fn_en_usb_read(pdfu, ret_data, ret_len);

  return ret;
}

static void fn_erase(dfu_dev_t *pdfu,
                     uint8_t method,
                     uint32_t page_addr,
                     uint32_t num_pages)
{
  int rsp;
  uint8_t command[12];
  uint8_t read_data_buffer[READ_DATA_SIZE] = {0};

  command[0] = CMD_ERASE;
  command[1] = method;
  command[2] = 0x00;
  command[3] = 0x00;
  memcpy(&command[4], (uint8_t *)&page_addr, 4);
  memcpy(&command[8], (uint8_t *)&num_pages, 4);

  rsp = command_transfer(pdfu,
                         command,
                         sizeof(command),
                         read_data_buffer,
                         READ_DATA_SIZE);

  if (rsp != 2)
    errx(EX_IOERR, "DFU_CMD_ERASE command response length error! (%d)", rsp);
  if (read_data_buffer[1] != CMD_BACK_SUCCESS)
    errx(EX_IOERR, "DFU_CMD_ERASE command failed! (error code: 0x%02X)",
         read_data_buffer[1]);
}

int fn_get_info(dfu_dev_t *pdfu, int8_t info_id, uint8_t *info)
{
  int rsp, data_len;
  uint8_t command[] = {CMD_GET_INFO, info_id};
  uint8_t read_data_buffer[READ_DATA_SIZE] = {0};

  if (info == NULL)
    return -1;

  rsp = command_transfer(pdfu,
                         command,
                         sizeof(command),
                         read_data_buffer,
                         READ_DATA_SIZE);

  if (rsp < 0)
    return -1;

  if (read_data_buffer[1] != CMD_BACK_SUCCESS)
  {
    errx(EX_PROTOCOL, "DFU_CMD_GET_INFO command failed! (error code: 0x%02X)",
         read_data_buffer[1]);
  }

  data_len = read_data_buffer[2];
  memcpy(info, &read_data_buffer[3], data_len);

  return data_len;
}

void fn_erase_chip(dfu_dev_t *pdfu)
{
  fn_erase(pdfu, 0xF0, 0x00, 0);
}

void fn_erase_pages(dfu_dev_t *pdfu, uint32_t page_addr, uint32_t num_pages)
{
  fn_erase(pdfu, 0x00, page_addr, num_pages);
}

void fn_program_page(dfu_dev_t *pdfu,
                     uint32_t page_addr,
                     uint8_t *data,
                     uint32_t offset,
                     uint32_t count)
{
  int rsp;
  uint8_t *command = dfufw_malloc(count + 12);
  uint8_t read_data_buffer[READ_DATA_SIZE] = {0};

  if (data == NULL)
  {
    free(command);
    return;
  }

  command[0] = CMD_WRITE_PAGE;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  memcpy(&command[4], (uint8_t *)&page_addr, 4);
  memcpy(&command[8], (uint8_t *)&count, 4);

  for (uint32_t idx = 0; idx < count; idx++)
    command[12 + idx] = data[offset + idx];

  rsp = command_transfer(pdfu,
                         command,
                         count + 12,
                         read_data_buffer,
                         READ_DATA_SIZE);

  if (rsp != 2)
    errx(EX_IOERR,
         "DFU_CMD_PROGRAM_PAGE command response length error! (%d)",
         rsp);
  if (read_data_buffer[1] != CMD_BACK_SUCCESS)
    errx(EX_IOERR,
         "DFU_CMD_PROGRAM_PAGE command failed! (error code: 0x%02X)",
         read_data_buffer[1]);

  free(command);
}

void fn_cmd_read(dfu_dev_t *pdfu,
                 uint32_t addr,
                 uint8_t *buffer,
                 uint32_t offset,
                 uint32_t count)
{
  int rsp;
  uint8_t command[10];
  uint8_t read_data_buffer[READ_DATA_SIZE] = {0};

  if (buffer == NULL)
    return;

  command[0] = CMD_READ;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  memcpy(&command[4], (uint8_t *)&addr, 4);
  memcpy(&command[8], (uint8_t *)&count, 2);

  rsp = command_transfer(pdfu,
                         command,
                         sizeof(command),
                         read_data_buffer,
                         READ_DATA_SIZE);

  if (rsp < 0)
    errx(EX_IOERR, "DFU_CMD_READ command response length error! (%d)", rsp);
  if (read_data_buffer[1] != CMD_BACK_SUCCESS)
    errx(EX_IOERR,
         "DFU_CMD_READ command failed! (error code: 0x%02X)",
         read_data_buffer[1]);

  memcpy(&buffer[offset], &read_data_buffer[2], count);
}

int fn_read_data(dfu_dev_t *pdfu,
                 uint32_t address,
                 uint8_t *buffer,
                 uint32_t count)
{
  uint32_t index = 0;

  if (buffer == NULL)
    return -1;

  while (index < count)
  {
    uint32_t read_len = count - index;
    if (read_len > 256)
      read_len = 256;

    fn_cmd_read(pdfu,
                address + index,
                buffer,
                (uint32_t)index,
                (uint32_t)read_len);
    index += read_len;
  }

  return 0;
}

void fn_reset(dfu_dev_t *pdfu, uint32_t delay_ms)
{
  int rsp;
  uint8_t command[8];
  uint8_t read_data_buffer[READ_DATA_SIZE] = {0};

  command[0] = CMD_RESET;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  memcpy(&command[4], (uint8_t *)&delay_ms, 4);

  rsp = command_transfer(pdfu,
                         command,
                         sizeof(command),
                         read_data_buffer,
                         READ_DATA_SIZE);
  if (rsp != 2)
    errx(EX_IOERR, "DFU_CMD_RESET command response length error! (%d)", rsp);
  if (read_data_buffer[1] != 0x00)
    errx(EX_IOERR,
         "DFU_CMD_RESET command failed! (error code: 0x%02X)",
         read_data_buffer[1]);
}

void fn_go(dfu_dev_t *pdfu, uint32_t address, uint32_t delay_ms)
{
  int rsp;
  uint8_t command[12];
  uint8_t read_data_buffer[READ_DATA_SIZE] = {0};

  command[0] = CMD_GO;
  command[1] = 0x00;
  command[2] = 0x00;
  command[3] = 0x00;
  memcpy(&command[4], (uint8_t *)&address, 4);
  memcpy(&command[4], (uint8_t *)&delay_ms, 4);

  rsp = command_transfer(pdfu,
                         command,
                         sizeof(command),
                         read_data_buffer,
                         READ_DATA_SIZE);
  if (rsp != 2)
    errx(EX_IOERR, "DFU_CMD_GO command response length error! (%d)", rsp);
  if (read_data_buffer[1] != 0x00)
    errx(EX_IOERR,
         "DFU_CMD_GO command failed! (error code: 0x%02X)",
         read_data_buffer[1]);
}
