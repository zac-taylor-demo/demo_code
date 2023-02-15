/*!
 * @file
 * log class header and associated constants.
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
 * File:   log.h
 * Author: busdev
 *
 * Created on 14 February 2023
 * Updated on 14 February 2023
 */

#ifndef LOG_H
#define LOG_H

// Error Codes.

#define NO_ERROR             0
#define TCP_PCB_MEMORY_ERR   1
#define TCP_BIND_ERR         2
#define TCP_START_LISTEN_ERR 3
#define TCP_BUFFER_ERR       4
#define TCP_WRITE_ERR        5
#define WIFI_INIT_ERR        6

// Error Messages.

#define UNDEFINED_ERROR_MSG      "Undefined error."
#define TCP_PCB_MEMORY_ERR_MSG   "Error creating PCB. Out of Memory."
#define TCP_BIND_ERR_MSG         "Unable to bind to port 80."
#define TCP_START_LISTEN_ERR_MSG "Out of memory while starting tcp_listen."
#define TCP_BUFFER_ERR_MSG       "Cannot send data, TCP send buffer too small."
#define TCP_WRITE_ERR_MSG        "Cannot send data, TCP write."
#define WIFI_INIT_ERR_MSG        "Failed to initialise WiFi module."

// Log Codes.

#define CONFIG_JUMPER_DETECTED         1
#define FLASH_STORAGE_AREA_INITIALISED 2
#define WIFI_CREDENTIALS_SET           3
#define WIFI_CREDENTIALS_UPDATED       4

#define UNDEFINED_LOG_MSG                  "Undefined message code."
#define CONFIG_JUMPER_DETECTED_MSG         "Configuration jumper detected."
#define FLASH_STORAGE_AREA_INITIALISED_MSG "Flash storage area initialised."
#define WIFI_CREDENTIALS_SET_MSG           "WiFi credentials set."
#define WIFI_CREDENTIALS_UPDATED_MSG       "WiFi credentials updated."

#include <iostream> 
#include <fstream>
#include <string>

using std::string;

class Log 
{
 public:
  Log(bool is_verbose);
  ~Log();

  string get_error_text(int error_number);
  string get_log_text(int log_number);
   
  void print_error(int error_number);
  void print_log(int log_number);
  void print_message(string msg);
 
 private:
  bool is_verbose;
};

#endif /* LOG_H */
