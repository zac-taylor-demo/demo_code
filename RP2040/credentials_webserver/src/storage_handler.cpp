/*!
 * @file
 * storage_handler classs.
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

//
// Storage Manager.
// 
// Manages the flash memory storage for the following variables:
//
// Variable             Length (Bytes)    Description
// --------             --------------    -----------
// Status                     1           Display controller's status
// WiFi SSN                   32          Network name
// WiFi Password              63          Password
// Image server's URL         2048        Server URL
//

/* 
 * File:   storage_handler.cpp
 * Author: busdev
 *
 * Created on 28 January 2023
 * Updated on 14 February 2023
 */

#include "storage_handler.h"

// PICO_FLASH_SIZE_BYTES # The total size of the RP2040 flash, in bytes
// FLASH_SECTOR_SIZE     # The size of one sector, in bytes (the minimum amount you can erase)
// FLASH_PAGE_SIZE       # The size of one page, in bytes (the mimimum amount you can write)

Storage_Handler::Storage_Handler():
 ssid(""),
 pass(""),
 server(""),
 epd_status(EPD_STORE_UNITIALISED)
 {
  initialise_storage();
 }

Storage_Handler::~Storage_Handler()
{ }

/*!
* \brief Initialises the non-volatile storage area for persistent application variables.
* 
* The last sector of flash memory is used to store the persistent application variables.
* The address of the start of the last sector is STORAGE_ADDR_START.
* Flash erase and write operations use STORAGE_OFFSET.
*
* A structure, store, holds the following variables:
*
* Name               Length (Bytes)      Description
* ----               --------------      -----------
* status                   1             The application's storage status
* wifi_ssid                33            WiFi SSID of local network (string)
* wifi_password            64            WiFi password of local network (string)
* image_server_url         2049          Remote image server's URL (string)
*
* Note: the strings have an extra byte for a trailing null terminator.
*
* Existing values are read as offsets from STORAGE_ADDR_START using the store fields.
* Both the instance variables: (ssid, pass, server, epd_status) and the new_store fields
* are set.
*
* New values are written to the new_store structure. 
* Then, the last sector is erased. 
* Lastly, the new_store fields are written to flash.
*
*/

void Storage_Handler::initialise_storage(void)
{
 uint32_t irq_enabled_status;

 store = (struct store*)(STORAGE_ADDR_START);  // Existing storage variables in flash.
 epd_status = store->status;
 new_store.status = epd_status;

 if ((epd_status == EPD_STORE_UNITIALISED) || (epd_status > EPD_STORE_CREDENTIALS_SET))
 {
  irq_enabled_status = save_and_disable_interrupts();
  flash_range_erase(STORAGE_OFFSET, FLASH_SECTOR_SIZE);

  epd_status = EPD_STORE_FORMATTED;
  new_store.status = epd_status;

// Initialise the strings storage space to zeros.

  memset(new_store.wifi_ssid,0,WIFI_SSID_LENGTH);
  memset(new_store.wifi_password,0,WIFI_PASSWORD_LENGTH);
  memset(new_store.image_server_url,0,IMAGE_SERVER_URL_LENGTH);

  flash_range_program(STORAGE_OFFSET, (const uint8_t*)&new_store, STORAGE_SIZE);

  epd_status = EPD_STORE_DEFAULT_VALUES;
  new_store.status = epd_status;

  restore_interrupts(irq_enabled_status);  // Enable interrupts.
 }
 else  // Read storage variables from flash into local variables and the new_store.
 {
  ssid.assign((const char*)store->wifi_ssid);
  pass.assign((const char*)store->wifi_password);
  server.assign((const char*)store->image_server_url);

  strcpy((char*)new_store.wifi_ssid, (const char*)store->wifi_ssid);
  strcpy((char*)new_store.wifi_password, (const char*)store->wifi_password);
  strcpy((char*)new_store.image_server_url, (const char*)store->image_server_url);
 }
}
  

/*!
* \brief Sets WiFi SSID.
*
* Assigns instance variable ssid.
* Initialise to zeros first.
*
* \param wifi_ssid
*/

void Storage_Handler::set_wifi_ssid(string wifi_ssid)
{
 memset(new_store.wifi_ssid,0,WIFI_SSID_LENGTH);
 memcpy(new_store.wifi_ssid, wifi_ssid.c_str(), wifi_ssid.length());
 ssid = wifi_ssid;
}

/*!
* \brief Sets WiFi password.
*
* Assigns instance variable pass.
* Initialise to zeros first.
*
* \param wifi_pswd
*/

void Storage_Handler::set_wifi_password(string wifi_pswd)
{
 memset(new_store.wifi_password,0,WIFI_PASSWORD_LENGTH);
 memcpy(new_store.wifi_password, wifi_pswd.c_str(), wifi_pswd.length());
 pass = wifi_pswd;
}

/*!
* \brief Sets image server's URL.
*
* Assigns instance variable server_url.
* Initialise to zeros first.
*
* \param server_url
*/

void Storage_Handler::set_image_server_url(string server_url)
{
 memset(new_store.image_server_url,0,IMAGE_SERVER_URL_LENGTH);
 memcpy(new_store.image_server_url, server_url.c_str(), server_url.length());
 server = server_url;
}
  
/*!
* \brief Sets display's status.
*
* Assigns instance variable epd_status.
*
* \param status
*/

void Storage_Handler::set_epd_status(uint8_t status)
{
 new_store.status = status;
 epd_status = status;
}
  
/*!
* \brief Gets WiFi network's SSID.
*
* \return string WiFi SSID.
*/

string Storage_Handler::get_wifi_ssid(void)
{
 return ssid;
}

/*!
* \brief Gets WiFi network's password.
*
* \return string WiFi password.
*/

string Storage_Handler::get_wifi_password(void)
{
 return pass;
}

/*!
* \brief Gets image server's URL.
*
* \return string server.
*/

string Storage_Handler::get_image_server_url(void)
{
 return server;
}

/*!
* \brief Gets display's status.
*
* \return uint8_t status.
*/

uint8_t Storage_Handler::get_epd_status(void)
{
 return epd_status;
}

/*!
* \brief The new_store fields are written to flash, from STORAGE_OFFSET.
*
* The offset relative to the start of the flash memory is STORAGE_OFFSET.
* Flash erase and write operations use STORAGE_OFFSET.
*
* Interrupts are disabled during flash erase and write operations.
* The last sector of flash is erased.
* The contents of the structure, new_store, are written to flash. 
* The number of bytes written must be a multiple of the page size (256 bytes).
* Once the flash operations are complete, interrupts are enabled.
*
*/

void Storage_Handler::write_data_to_store(void)
{
 uint32_t irq_enabled_status;

 irq_enabled_status = save_and_disable_interrupts();
 flash_range_erase(STORAGE_OFFSET, FLASH_SECTOR_SIZE);
 flash_range_program(STORAGE_OFFSET, (const uint8_t*)&new_store, STORAGE_SIZE);
 restore_interrupts(irq_enabled_status);
}
