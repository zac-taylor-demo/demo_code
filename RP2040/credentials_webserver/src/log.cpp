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
 is_verbose(is_verbose) {}

void Log::print_error(int error_number)
{
 if (is_verbose == true)
 {
  print_message("");  
  
  switch (error_number)
  {
   case NO_ERROR: break;
  
   default: print_message(UNDEFINED_ERROR_MSG);
  }
 }
}

void Log::print_log(int log_number)
{
 if (is_verbose == true)
 {
  print_message("");   
  
  switch (log_number)
  {
  
   default: print_message(UNDEFINED_LOG_MSG);
  }
 }
}

void Log::print_message(string msg)
{
 if (is_verbose == true)
 {
  std::cout << msg << std::endl;
 }
}