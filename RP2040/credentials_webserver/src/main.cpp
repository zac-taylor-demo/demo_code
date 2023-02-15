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
 * File:   main.cpp
 * Author: Z Taylor
 *
 * Created on 30 December 2022
 * Updated on 14 February 2023
 * 
 * Description
 * -----------
 * Demonstration application for the Pico-W.
 * 
 * Goal: Accept WiFi credentials and a remote server URL via a web browser
 * connected to a local webserver running on the Pico-W.
 * 
 * The Pico-W is set up as a WiFi access point that a web client (browser) 
 * connects to.
 * Once a connection is establised, the Pico-W's local webserver responds to
 * web client requests with configuration pages.
 * 
 * Background
 * ----------
 * This application is based on part of an existing ESP8266 application 
 * that acts as a WiFi access point and webserver.
 * The Pico-W was considered possible alternative to the ESP8266.
 * 
 * Building a Webserver using TCP Sockets
 * --------------------------------------
 * The Pico-W SDK does not have a suitable webserver library 
 * (at the time of writing).
 * Therefore, a basic HTTP message handler was built on top of the existing
 * LwIP library, using a TCP socket that accepts connections on port 80.
 * This has the benefit of keeping the code size relatively small, as size
 * is a constraint on the Pico-W.
 * Note: the default lwip options were used.
 * 
 * Development Environment
 * -----------------------
 * IDE: Visual Studio Code with the C/C++, Cortex-Debug and CMake extensions.
 * SDK: pico-SDK (RP2040).
 * Compiler: GCC 8.3.1 arm-none-eabi
 * Debugging via SWD and picoprobe. UART output (printf) via USB.
 * 
 */

#define GPIO15 15

#include <string.h>
#include <stdlib.h>

#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "dhcpserver.h"
#include "storage_handler.h"
#include "log.h"
#include "credentials_webserver.h"

cyw43_t cyw43_state;

/*!
* \brief Starts local webserver and polls for WiFi activity.
*
* The Pico-W acts as a webserver to allow display configuration
* parameters to be entered via a web client.
* The Credentials_Webserver object handles client requests.
* When the user exits "Configuration Mode", the webserver is 
* is shutdown and the system enters "Display Mode".
*
*/

void run_server(Storage_Handler *sh, Log *log) 
{
 cws = new Credentials_Webserver(sh, log);
 cws->set_is_configuring(true);
 cws->start_webserver();

 for (;;) // Poll for WiFi activity.
 {
  cyw43_arch_poll();
  sleep_ms(1);  // 1

// Check display mode and stop web server when mode changes from
// configuration to display.

  if (cws->get_is_configuring() == false)
  {
   break;
  }
 }
}

/*!
* \brief Application code entry point.
*
* Initialises I/O.
* Initialises WiFi module and creates an access point.
*
* Access point credentials:
* SSID    : APSSID
* Password: APPSK
* Cipher  : CYW43_AUTH_WPA2_AES_PSK
*
* Starts a DHCP server:
* Gateway: 192.168.4.1
* Mask   : 255.255.255.0
*
* Starts local webserver.
*
*/

int main() 
{
 string log_text;
 ip4_addr_t gw, mask;
 dhcp_server_t dhcp_server;
 Storage_Handler *sh;
 Log *log;

 stdio_init_all();

// GPIO15 is set as an input pin that is pulled up.
// On the hardware side, it can be jumpered to ground, causing the Pico-W to
// act as a wireless access point.

 gpio_init(GPIO15);
 gpio_pull_up(GPIO15);

 log = new Log(true); // Verbose output.
 sh = new Storage_Handler();

// Check the EPD status byte and the level of GPIO15 (false = LOW).

 if ((sh->get_epd_status() != EPD_STORE_CREDENTIALS_SET) || (gpio_get(GPIO15) == false))
 {
  if (cyw43_arch_init()) 
  {
   log->print_error(WIFI_INIT_ERR);
   return 1;
  }

  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);  // Turn on LED.
  cyw43_arch_enable_ap_mode(APSSID, APPSK, CYW43_AUTH_WPA2_AES_PSK);
  cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 20, 1, 1, 1));

// Setup DHCP server.

  IP4_ADDR(&gw, 192, 168, 4, 1);
  IP4_ADDR(&mask, 255, 255, 255, 0);
  dhcp_server_init(&dhcp_server, &gw, &mask);  // Start the DHCP server.

  run_server(sh, log);

  log->print_message("\nEntering Display Mode.\n");  // *** Debug ***
 }
 else
 {

// *** Start of Test Section ***

  string log_text;

  string store_ssid       = sh->get_wifi_ssid();
  string store_password   = sh->get_wifi_password();
  string store_server_url = sh->get_image_server_url();

  log_text = "\n SSID:       " + store_ssid + "\n Password:   " + store_password + "s\n Server URL: " + store_server_url + "\n";
  log->print_message(log_text);
  log->print_message("\nWiFi credentials already set, leaving...\n");

// *** End of Test Section ***

 }
}
