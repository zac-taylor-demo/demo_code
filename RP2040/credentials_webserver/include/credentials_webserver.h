/*!
 * @file
 * credentials_webserver class header and associated constants.
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
 * File:   credentials_webserver.h
 * Author: busdev
 *
 * Created on 30 December 2022
 * Updated on 31 January 2023
 */

#ifndef __CREDENTIALS_WEBSERVER_H__
#define __CREDENTIALS_WEBSERVER_H__

#define APSSID "EPD_Init"
#define APPSK  "epdsetup"

#define MIN_SSID_LENGTH     1
#define MAX_SSID_LENGTH     32
#define MIN_PASSWORD_LENGTH 8
#define MAX_PASSWORD_LENGTH 63
#define MAX_URL_LENGTH      150
#define MAX_CONTENTS_LENGTH 2048

#define HTTP_GET_OFFSET  4    // Used by extract_path.
#define HTTP_POST_OFFSET 5    // Used by extract_path.

#define HTTP_HEADER_BUFFER_SIZE 250

#define HEADER1 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:320px;border:2px solid blue;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER2 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid blue;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER3 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid blue;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER4 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid red;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER5 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid gray;border-radius:4px;text-align:center;background-color:rgb(200,200,200)\">"
#define HEADER6 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid red;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER7 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid blue;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER8 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid blue;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"
#define HEADER9 "<div align=\"center\"><div style=\"margin:auto;height:auto;max-width:300px;border:2px solid red;border-radius:4px;text-align:center;background-color:rgb(100,150,150)\">"

#define FOOTER1 "</div></div>"
#define FORM1   "<form method=\"POST\">"
#define BUTTON1 "<input type=\"submit\" style=\"margin:10px;padding:5px 10px;border:1px solid gray;border-radius:4px;background-color:rgb(190,190,190)\" "
#define BUTTON2 "<input type=\"submit\" style=\"margin:10px;padding:5px 10px;border:1px solid brown;border-radius:4px;background-color:rgb(255,0,0)\" "

#define PAGE_TITLE          "<title>EPD Setup</title>"
#define TITLE1              "<H3>DISPLAY SETUP</H3>"
#define MAIN_MENU           "<H4>Main Menu</H4>"
#define WIFI_TITLE          "<H4>Image Server Credentials</H4>"
#define ERROR_TITLE         "<H3>ERROR</H3>"
#define DEVICE_ID_TITLE     "<H4>Display ID</H4>"
#define DISPLAY_MODE_TITLE  "<H3>Enter Display Mode</H3>"
#define RESET_DISPLAY_TITLE "<H3>Reset Display</H3>"
#define PAGE_NOT_FOUND      "<H1>Page Not Found</H1>"

// SSID rules:
//
// 1. First character must not be in ['!', '#', ';'].
// 2. Following characters NOT allowed ['+', ']', '/', '"', TAB].
// 3. Trailing spaces NOT allowed.

#define SSID_ERROR    "<b>Invalid Network Name (SSID)</b><br><br><span><i>SSID must be:<br>1 to 32 characters long<br>not start with (!, #, ;)<br>not contain (+, ], /,\")<br>not have trailing spaces.</i></span>"
#define PASS_ERROR    "<b>Invalid Password</b><br><br><i>Password must be between 8 and 63<br>ASCII printable characters in length</i>"
#define URL_ERROR     "<b>Invalid URL</b><br><br><i>URL must not have spaces and<br>be correctly formed</i>"
#define REQUEST_ERROR "<b>Invalid Request</b>"
#define STORAGE_ERROR "<b>Unable to store user entered data</b>"

// Arguments for WiFi credentials and Image Server URL.

#define NETWORK_NAME_ARGUMENT "networkname"
#define PASSWORD_ARGUMENT     "password"
#define SERVER_URL_ARGUMENT   "serverURL"

enum http_req_type { HTTP_GET, HTTP_POST, HTTP_UNKNOWN };

#define HTTP_PORT 80

#include <string>
#include <cstring>
#include <assert.h>

using std::string;

#include "lwip/tcp.h"
#include "log.h"
#include "storage_handler.h"

extern err_t w_http_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
extern err_t w_http_sent_callback(void *arg, struct tcp_pcb *pcb, u16_t len);
extern err_t http_accept_callback(void *arg, struct tcp_pcb *pcb, err_t err);

class Credentials_Webserver 
{
 public:
  Credentials_Webserver(Storage_Handler *sh, Log *log);
  ~Credentials_Webserver();

  void set_is_configuring(bool configuring_display);
  bool get_is_configuring(void);
  
  err_t generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len);
  int generate_http_header(char *buf, const char *fext, int fsize);
  void start_webserver();
  void stop_webserver(struct tcp_pcb *pcb);

  err_t http_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
  err_t http_sent_callback(void *arg, struct tcp_pcb *pcb, u16_t len);

 private:
  string replace_special_html_characters(string source_str);
  bool check_wifi_ssid_format(void);
  bool check_wifi_password_format(void);
  bool check_image_server_url_format(void);
  bool check_fields(void);
  
  err_t send_page(struct tcp_pcb *pcb, const char *data_ptr, int data_len);
  err_t send_data(struct tcp_pcb *pcb, const char *data_ptr, int data_len);

  err_t handle_page_not_found(struct tcp_pcb *pcb);
  err_t handle_home_page(struct tcp_pcb *pcb);
  err_t handle_image_server_page(struct tcp_pcb *pcb);
  err_t handle_device_id_page(struct tcp_pcb *pcb);
  err_t handle_master_reset_page(struct tcp_pcb *pcb);
  err_t handle_reset_confirmed_page(struct tcp_pcb *pcb);
  err_t handle_image_server_credentials_page(struct tcp_pcb *pcb, char *req, int rlen);
  err_t handle_reset_image_server_credentials_page(struct tcp_pcb *pcb);
  err_t handle_cancel_image_server_credentials_page(struct tcp_pcb *pcb);
  err_t handle_change_display_mode_page(struct tcp_pcb *pcb);
  err_t handle_setup_display_mode_page(struct tcp_pcb *pcb);
  err_t handle_error_message_page(struct tcp_pcb *pcb, string error_message, string web_directory);
  err_t handle_http_post(struct tcp_pcb *pcb, char *req, int rlen);
  err_t handle_http_get(struct tcp_pcb *pcb, const char *req, int rlen);

  enum http_req_type decode_http_request(const char *req);

  void extract_path(char *path, const char *req, int rlen, int initial_offset);
  string extract_argument(const char* data, int len, const char* argument_name);
   
  Storage_Handler *sh;
  Log *log;

  string log_text;
  string new_ssid;
  string new_pass;
  string new_server;
  string ssid;
  string pass;
  string server;

  string web_page_header;
  string web_page_footer;

  bool is_display_reset;
  bool is_master_reset_error;
  bool is_configuring;

  bool is_ssid_present_and_correct;
  bool is_password_present_and_correct;
  bool is_server_url_present_and_correct;
 };

 extern Credentials_Webserver *cws;

#endif

