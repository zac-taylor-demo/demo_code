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

#define NO_ERROR 0

// Error Messages.

#define UNDEFINED_ERROR_MSG "Undefined error."

// Log Codes.

#define CONFIG_JUMPER_DETECTED         1
#define FLASH_STORAGE_AREA_INITIALISED 2
#define WIFI_CREDENTIALS_SET           3
#define WIFI_CREDENTIALS_UPDATED       4

#define UNDEFINED_LOG_MSG "Undefined message code."

#include <iostream> 
#include <fstream>
#include <string>

using std::string;

class Log 
{
 public:
  Log(bool is_verbose);
   
  void print_error(int error_number);
  void print_log(int log_number);
  void print_message(string msg);
 
 private:
  bool is_verbose;
};

#endif /* LOG_H */
