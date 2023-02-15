/*!
 * @file
 * storage_handler class header and associated constants.
 */

/*
 * Copyright (c) 2023, FAV Software Limited. All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/* 
 * File:   storage_handler.h
 * Author: busdev
 *
 * Created on 28 January 2023
 * Updated on 14 February 2023
 */

#ifndef __STORAGE_HANDLER_H__
#define __STORAGE_HANDLER_H__

// Storage states.

#define EPD_STORE_UNITIALISED         0
#define EPD_STORE_FORMATTED           1
#define EPD_STORE_DEFAULT_VALUES      2
#define EPD_STORE_SETTING_CREDENTIALS 3
#define EPD_STORE_CREDENTIALS_SET     4

// Error codes.

#define SH_OK     0
#define SH_ERROR -1

// Flash memory.

// PICO_FLASH_SIZE_BYTES # 2MB.  The total size of the RP2040 flash, in bytes
// FLASH_SECTOR_SIZE     # 4096. The size of one sector, in bytes (the minimum amount you can erase)
// FLASH_PAGE_SIZE       # 256.  The size of one page, in bytes (the mimimum amount you can write)

#define STORAGE_OFFSET PICO_FLASH_SIZE_BYTES - (1 * FLASH_SECTOR_SIZE)
#define STORAGE_ADDR_START (XIP_BASE + STORAGE_OFFSET)

#define WIFI_SSID_LENGTH     33
#define WIFI_PASSWORD_LENGTH 64
#define IMAGE_SERVER_URL_LENGTH 2049
#define ERROR_CODES_SIZE 10
#define LOG_CODES_SIZE   10
#define STORAGE_PADDING 135          // Padding to make structure size multiple of page size.
#define STORAGE_SIZE 2304            // Divisible by page size (256).


#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#include <string>
#include <cstring>

using std::string;

// Storage variables.

struct store 
{
 uint8_t status;
 uint8_t wifi_ssid[WIFI_SSID_LENGTH];
 uint8_t wifi_password[WIFI_PASSWORD_LENGTH];
 uint8_t image_server_url[IMAGE_SERVER_URL_LENGTH];
 uint8_t error_codes[ERROR_CODES_SIZE];
 uint8_t error_codes_index;
 uint8_t log_codes[LOG_CODES_SIZE];
 uint8_t log_codes_index;
 uint8_t padding[STORAGE_PADDING];
};


class Storage_Handler 
{
 public:
  Storage_Handler();
  ~Storage_Handler();

  void initialise_storage(void);

  void set_wifi_ssid(string wifi_ssid);
  void set_wifi_password(string wifi_pswd);
  void set_image_server_url(string server_url);
  void set_epd_status(uint8_t status);

  string get_wifi_ssid(void);
  string get_wifi_password(void);
  string get_image_server_url(void);
  uint8_t get_epd_status(void);

  void write_data_to_store(void);

 private:
  string ssid;
  string pass;
  string server;
  uint8_t epd_status;

  struct store* store;
  struct store new_store;

};

#endif