/*!
 * @file
 * log class.
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
// Log.
// 

/* 
 * File:   log.cpp
 * Author: busdev
 *
 * Created on 14 February 2023
 * Updated on 14 February 2023
 */

#include "log.h"

Log::Log(bool is_verbose):
 is_verbose(is_verbose) { }

Log::~Log()
{ }

/*!
* \brief Gets error text using error number.
*
* \param error_number
* \return string
*/

string Log::get_error_text(int error_number)
{
 string error_text = "";

 switch (error_number)
 {
  case NO_ERROR: break;
  case TCP_PCB_MEMORY_ERR: error_text = TCP_PCB_MEMORY_ERR_MSG; break;
  case TCP_BIND_ERR: error_text = TCP_BIND_ERR_MSG; break;
  case TCP_START_LISTEN_ERR: error_text = TCP_START_LISTEN_ERR_MSG; break;
  case TCP_BUFFER_ERR: error_text = TCP_BUFFER_ERR_MSG; break;
  case TCP_WRITE_ERR: error_text = TCP_WRITE_ERR_MSG; break;
  case WIFI_INIT_ERR: error_text = WIFI_INIT_ERR_MSG; break;
  
  default: error_text = UNDEFINED_ERROR_MSG;
 }

 return error_text;
}

/*!
* \brief Gets log text using log number.
*
* \param log_number
* \return string
*/

string Log::get_log_text(int log_number)
{
 string log_text = "";

 switch (log_number)
 {
  case CONFIG_JUMPER_DETECTED: log_text = CONFIG_JUMPER_DETECTED_MSG; break;
  case FLASH_STORAGE_AREA_INITIALISED: log_text = FLASH_STORAGE_AREA_INITIALISED_MSG; break;
  case WIFI_CREDENTIALS_SET: log_text = WIFI_CREDENTIALS_SET_MSG; break;
  case WIFI_CREDENTIALS_UPDATED: log_text = WIFI_CREDENTIALS_UPDATED_MSG; break;

  default: log_text = UNDEFINED_LOG_MSG;
 }

 return log_text;
}

/*!
* \brief Prints error message.
*
* Looks up text of error message using the error_number.
*
* \param error_number
*/

void Log::print_error(int error_number)
{
 if (is_verbose == true)
 {
  print_message("");  
  
  switch (error_number)
  {
   case NO_ERROR: break;
   case TCP_PCB_MEMORY_ERR: print_message(TCP_PCB_MEMORY_ERR_MSG); break;
   case TCP_BIND_ERR: print_message(TCP_BIND_ERR_MSG); break;
   case TCP_START_LISTEN_ERR: print_message(TCP_START_LISTEN_ERR_MSG); break;
   case TCP_BUFFER_ERR: print_message(TCP_BUFFER_ERR_MSG); break;
   case TCP_WRITE_ERR: print_message(TCP_WRITE_ERR_MSG); break;
   case WIFI_INIT_ERR: print_message(WIFI_INIT_ERR_MSG); break;
  
   default: print_message(UNDEFINED_ERROR_MSG);
  }
 }
}

/*!
* \brief Prints log message.
*
* Looks up text of log message using the log_number.
*
* \param log_number
*/

void Log::print_log(int log_number)
{
 if (is_verbose == true)
 {
  print_message("");   
  
  switch (log_number)
  {
   case CONFIG_JUMPER_DETECTED: print_message(CONFIG_JUMPER_DETECTED_MSG); break;
   case FLASH_STORAGE_AREA_INITIALISED: print_message(FLASH_STORAGE_AREA_INITIALISED_MSG); break;
   case WIFI_CREDENTIALS_SET: print_message(WIFI_CREDENTIALS_SET_MSG); break;
   case WIFI_CREDENTIALS_UPDATED: print_message(WIFI_CREDENTIALS_UPDATED_MSG); break;

   default: print_message(UNDEFINED_LOG_MSG);
  }
 }
}

/*!
* \brief Prints message.
*
* Checks the is_verbose boolean and if true, prints msg to stdout.
*
* \param msg
*/

void Log::print_message(string msg)
{
 if (is_verbose == true)
 {
  std::cout << msg << std::endl;
 }
}