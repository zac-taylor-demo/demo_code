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
// WebServer
//
// Local web server for display configuration.
//
// URL: http://192.168.4.1
//
// Directories
// -----------
// /                                   Home page.
// /setup/home                         Home page.
// /setup/imageserver                  WiFi credentials amd image server URL entry.
// /setup/imageservercredentials       Store wifi credentials and image server URL.
// /setup/resetimageservercredentials  Clears WiFi and image server URL input fields.
// /setup/cancelimageservercredentials Restores "new" ssid, pass and server to original values.
// /setup/deviceid                     Displays device ID.
// /setup/display                      Allows switch to display mode.
// /setup/displaymode                  Switches to display mode.
// /setup/masterreset                  Displays confirm reset page.
// /setup/resetconfirmed               Restores display to factory defaults.

//
// The display has two operating modes: DISPLAY and CONFIGURATION.
// The display's website is only active in the CONFIGURATION operating mode.
//

/* 
 * File:   credentials_webserver.cpp
 * Author: busdev
 *
 * Created on 30 December 2022
 * Updated on 31 January 2023
 */

#include "credentials_webserver.h"

Credentials_Webserver *cws;

Credentials_Webserver::Credentials_Webserver(Storage_Handler *sh, Log *log):
 sh(sh),
 log(log),
 log_text(""),
 new_ssid(""),
 new_pass(""),
 new_server(""),
 ssid(""),
 pass(""),
 server(""),
 is_display_reset(false),
 is_master_reset_error(false),
 is_configuring(false),
 is_ssid_present_and_correct(false),
 is_password_present_and_correct(false),
 is_server_url_present_and_correct(false)
 {
  web_page_header = "<html><head>";
  web_page_header = web_page_header + PAGE_TITLE + "<style type=\"text/css\"></style></head>";
  web_page_footer = FOOTER1;
  web_page_footer = web_page_footer + "</body></html>";

// Retrieve existing credentials.

  ssid = sh->get_wifi_ssid();
  pass = sh->get_wifi_password();
  server = sh->get_image_server_url();

// Set new version of credentials to existing credentials.

  new_ssid = ssid;
  new_pass = pass;
  new_server = server;

// Check credentials for correctness.

  is_ssid_present_and_correct = check_wifi_ssid_format();
  is_password_present_and_correct = check_wifi_password_format();
  is_server_url_present_and_correct = check_image_server_url_format();
 }

Credentials_Webserver::~Credentials_Webserver()
{ }

/*!
* \brief Sets is_configuring boolean.
*
* \param configuring_display
*/

void Credentials_Webserver::set_is_configuring(bool configuring_display)
{
 is_configuring = configuring_display;
}

/*!
* \brief Gets is_configuring boolean.
*
* \return bool
*/

bool Credentials_Webserver::get_is_configuring(void)
{
 return is_configuring;
}

/*!
* \brief Replaces special characters in HTML string.
*
* Replaces '+' with ' ' and %nn with the ASCII character.
* Note: the 'special' characters are introduced by the client
* when the browser encounters user input characters that have
* a special meaning e.g. ' ' and '/'.
*
* \param source_str Input string from web client.
* \return string.
*/

string Credentials_Webserver::replace_special_html_characters(string source_str)
{
 string output_str = "";
 int source_len = source_str.length();
 uint8_t high_nibble;
 uint8_t low_nibble;
 uint8_t ascii_value;
 char current_char;

 for (int i = 0; i < source_len; i++)
 {
  if (source_str[i] == '+')
  {
   output_str += " ";  // Replace '+' with ' '.
  }
  else if (source_str[i] == '%') // Next two characters are the hex value for an ascii character.
  {
   current_char = source_str[++i];

   if ((current_char >= '0') && (current_char <= '9'))
   {
    high_nibble = (uint8_t)(current_char - '0');
   }
   else  // Character between 'A' and 'F'.
   {
    high_nibble = (uint8_t)((current_char - 'A') + 10);
   }

   current_char = source_str[++i];

   if ((current_char >= '0') && (current_char <= '9'))
   {
    low_nibble = (uint8_t)(current_char - '0');
   }
   else  // Character between 'A' and 'F'.
   {
    low_nibble = (uint8_t)((current_char - 'A') + 10);
   }

   ascii_value = (high_nibble * 16) + low_nibble;
   output_str += (char)ascii_value;
  }
  else
  {
   output_str += source_str[i];
  }
 }

 return output_str;
}

/*!
* \brief Checks the WiFi SSID.
*
* The WiFi SSID's length must be between 1 and 32 characters (bytes).
*
* Rules
* -----
* 1. First character must not be in ['!', '#', ';'].
* 2. Following characters NOT allowed ['+', ']', '/', '"', TAB].
* 3. Trailing spaces NOT allowed.
* 4. Each character must be 'ASCII printable' i.e. in the decimal range 32..126.
*
* \return Boolean.
*/

bool Credentials_Webserver::check_wifi_ssid_format(void)
{
 int ssid_len;
 uint8_t ssid_char;
 bool is_valid_characters = false;

 new_ssid = replace_special_html_characters(new_ssid);
 ssid_len = new_ssid.length();

 if ((ssid_len >= MIN_SSID_LENGTH) && 
     (ssid_len <= MAX_SSID_LENGTH))
 {
  if ((new_ssid[0] != '!') && (new_ssid[0] != '#') && (new_ssid[0] != ';'))  // Illegal start character.
  {
   if (new_ssid[ssid_len - 1] != ' ')  // Trailing space NOT allowed
   {
    if ((new_ssid.find("+") == std::string::npos)  &&
        (new_ssid.find("]") == std::string::npos)  &&
        (new_ssid.find("/") == std::string::npos)  &&
        (new_ssid.find("\"") == std::string::npos) &&
        (new_ssid.find("\t") == std::string::npos))
    {
     is_valid_characters = true;

     for (int i = 0; i < ssid_len; i++)
     {
      ssid_char = (uint8_t)new_ssid[i];

      if ((ssid_char < 32) || (ssid_char > 126))
      {
       is_valid_characters = false;
       break;
      }
     }
    }
   }
  }
 }

 return is_valid_characters;
}

/*!
* \brief Checks the WiFi password (passphrase).
*
* The WiFi password's length must be between 8 and 63 characters (bytes).
* Each character must be 'ASCII printable' i.e. in the decimal range 32..126.
*
* \return Boolean.
*/

bool Credentials_Webserver::check_wifi_password_format(void)
{
 int password_len;
 uint8_t password_char;
 bool is_valid_characters = false;

 new_pass = replace_special_html_characters(new_pass);
 password_len = new_pass.length();

 if ((password_len >= MIN_PASSWORD_LENGTH) && 
     (password_len <= MAX_PASSWORD_LENGTH))
 {
  is_valid_characters = true;

  for (int i = 0; i < password_len; i++)
  {
   password_char = (uint8_t)new_pass[i];

   if ((password_char < 32) || (password_char > 126))
   {
    is_valid_characters = false;
    break;
   }
  }
 }

 return is_valid_characters;
}

/*!
* \brief Checks the image server's URL format.
*
* The image server's URL must not contain any spaces.
* Each character must be 'ASCII printable' i.e. in the decimal range 32..126.
*
* \return Boolean.
*/

bool Credentials_Webserver::check_image_server_url_format(void)
{
 int server_url_len;
 uint8_t server_url_char;
 bool is_valid_characters = false;

 new_server = replace_special_html_characters(new_server);
 server_url_len = new_server.length();

 if (new_server.find(" ") == std::string::npos)
 {
  is_valid_characters = true;

  for (int i = 0; i < server_url_len; i++)
  {
   server_url_char = (uint8_t)new_server[i];

   if ((server_url_char < 32) || (server_url_char > 126))
   {
    is_valid_characters = false;
    break;
   }
  }
 }

 return is_valid_characters;
}

/*!
* \brief Checks the image server's credentials are present, and are not zero length.
*
* \return Boolean.
*/

bool Credentials_Webserver::check_fields(void)
{
 bool fields_valid = false;

  if ((is_ssid_present_and_correct == true)       &&
      (is_password_present_and_correct == true)   &&
      (is_server_url_present_and_correct == true) &&
      (new_ssid.length() > 0) &&
      (new_pass.length() > 0) &&
      (new_server.length() > 0))
 {
  fields_valid = true;
 }

 return fields_valid;
}

/*!
* \brief Sends an HTML page to the client.
*
* The page consists of a header and contents.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param data_ptr Pointer to the page contents.
* \param data_len Number of bytes in contents.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::send_page(struct tcp_pcb *pcb, const char *data_ptr, int data_len)
{
 err_t err = ERR_OK;
 int hlen;
 char http_header_buf[HTTP_HEADER_BUFFER_SIZE];
 char lbuf[40];

// Check the parameters.

 if ((!pcb) || (!data_ptr) || (data_len > MAX_CONTENTS_LENGTH))
  err = ERR_ARG;

// Create header, setting content length.

 if (err == ERR_OK)
 {
  strcpy(http_header_buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n");
  sprintf(lbuf, "Content-length: %d\r\n", data_len);
  strcat(http_header_buf, lbuf);
  strcat(http_header_buf, "Connection: close\r\n\r\n");
  hlen = strlen(http_header_buf);
 
  err = send_data(pcb, http_header_buf, hlen);  // Send data (header) to client.

  if (err == ERR_OK)
  {
   err = send_data(pcb, data_ptr, data_len);    // Send data (contents) to client.
  }
 }

 if ((err != ERR_OK) && (pcb))  // Stop the web server if error occurred...
 {
  stop_webserver(pcb);
 }

 return err;
}

/*!
* \brief Sends an HTML data to the client.
*
* Called by send_page().
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param data_ptr Pointer to the data to be sent.
* \param data_len Number of bytes in data.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::send_data(struct tcp_pcb *pcb, const char *data_ptr, int data_len)
{
 err_t err = ERR_OK;

// Check the parameters.

 if ((!pcb) || (!data_ptr) || (data_len > MAX_CONTENTS_LENGTH))
  return ERR_ARG;

// Check the tcp buffer size.

 if (tcp_sndbuf(pcb) < data_len) 
 {
  log->print_message("Cannot send data, TCP send buffer too small\r\n");

//  printf("Cannot send data, tcp_sndbuf = %d bytes, data length = %d bytes\r\n",
//         tcp_sndbuf(pcb), data_len);   // *** Debug ***  ??

  err = ERR_MEM;
 }

 if (err == ERR_OK)
 {
  err = tcp_write(pcb, data_ptr, data_len, TCP_WRITE_FLAG_COPY);

  if (err != ERR_OK) 
  {
   log->print_message("Error writing test HTTP header\r\n");

//   printf("error (%d) writing test http header\r\n", err);  // *** Debug ***
  }
 }

 return err;
}

/*!
* \brief Sends page not found to the client.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_page_not_found(struct tcp_pcb *pcb)
{
 string web_page_str;

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER5 + PAGE_NOT_FOUND + " </body> </html>";

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Sends home page to the client.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_home_page(struct tcp_pcb *pcb)
{
 string web_page_str;

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body>" + HEADER1 + TITLE1 + MAIN_MENU + FORM1;
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/imageserver\" value=\"Image Server\">&nbsp;&nbsp;";
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/deviceid\" value=\"Device ID\">";

// Test wifi credentials. If O.K, show display button.

 if (check_fields() == true)
 {
  web_page_str = web_page_str  + "&nbsp;&nbsp;" + BUTTON1 + "formaction=\"/setup/display\" value=\"Display\">";
 }
 
 web_page_str = web_page_str + "<br><br>" + BUTTON2 + "formaction=\"/setup/masterreset\" value=\"Master Reset\"></form><br>";
 web_page_str = web_page_str + web_page_footer;

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Sends image server page to the client.
*
* Display image server credentials page.
* Include buttons to save, reset and cancel.
*
* Maximum length of input fields
* ------------------------------
* Network name  32
* Password      63
* Server URL    2048
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_image_server_page(struct tcp_pcb *pcb)
{
 string web_page_str;

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER3 + TITLE1 + WIFI_TITLE + FORM1;
 web_page_str = web_page_str + "<input type=\"text\" name=\"networkname\" placeholder=\"Network Name\" maxlength=\"32\" autofocus";
 web_page_str = web_page_str + " value=\"" + new_ssid + "\"><br><br>";
 web_page_str = web_page_str + "<input type=\"password\" name=\"password\" placeholder=\"Password\" maxlength=\"63\"";
 web_page_str = web_page_str + " value=\"" + new_pass + "\"><br><br>";
 web_page_str = web_page_str + "<input type=\"text/plain\" name=\"serverURL\" placeholder=\"Image Server URL\" maxlength=\"2048\"";
 web_page_str = web_page_str + " value=\"" + new_server + "\"><br><br>";
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/imageservercredentials\" value=\"Save\">&nbsp;&nbsp;";
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/resetimageservercredentials\" value=\"Reset\">&nbsp;&nbsp";
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/cancelimageservercredentials\" value=\"Cancel\"></form>";
 web_page_str = web_page_str + web_page_footer;

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Sends display ID page to the client.
*
* Unique display ID used in configuration of host server
* for identifying the display.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_device_id_page(struct tcp_pcb *pcb)
{
 string web_page_str;
 string label_id = "Test_Label_ID";   // *** Test *** Outstanding Work ***

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER2 + TITLE1 + DEVICE_ID_TITLE;
 web_page_str = web_page_str + "<p>Use the display ID to identify this<br>display in the image server configurator.<br> ";
 web_page_str = web_page_str + "<br><b><span style=\"font-family:courier\">" + label_id + "</span></b></p>";
 web_page_str = web_page_str + FORM1 + BUTTON1 + "formaction=\"/setup/home\" value=\"OK\">";
 web_page_str = web_page_str + web_page_footer;

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Sends master reset page to the client.
*
* Allow user to reset the display settings to their default
* values.
* Displays confirmation button.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_master_reset_page(struct tcp_pcb *pcb)
{
 string web_page_str;

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER6 + TITLE1 + RESET_DISPLAY_TITLE;
 web_page_str = web_page_str + "<p>Press Confirm to reset the display<br>to factory defaults.</p>";
 web_page_str = web_page_str + FORM1 + BUTTON1 + "formaction=\"/setup/resetconfirmed\" value=\"Confirm\">&nbsp;&nbsp";
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/home\" value=\"Cancel\"></form>";
 web_page_str = web_page_str + web_page_footer;

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Processes user master reset confirmation.
*
* Attempts to reset the display to it's default configuration.
* If successful, displays page indicating display was indeed reset,
* otherwise display page informing user the display was NOT reset.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_reset_confirmed_page(struct tcp_pcb *pcb)
{
 err_t err = ERR_OK;
 string web_page_str;

 bool result = false;

 if (!pcb)
  return ERR_ARG;

// *** Outstanding Work ***
// result = master_reset(); // Main display reset function, located in FileManager.
 result = true;  // *** Test ***

 if (result == true)
 {
  is_display_reset = true;

  new_ssid   = ssid;
  new_pass   = pass;
  new_server = server;
 }
 else
 {
  is_master_reset_error = true;
 }
 
 if (is_master_reset_error == true) // Show display failed to reset page.
 {
  web_page_str = web_page_header;
  web_page_str = web_page_str + "<body>" + HEADER4 + TITLE1;
  web_page_str = web_page_str + "<p>Unable to reset the display to<br>factory defaults</p>";
  web_page_str = web_page_str + FORM1 + BUTTON1 + "formaction=\"/setup/home\" value=\"OK\"></form>";
  web_page_str = web_page_str + web_page_footer;

  err = send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
 }
 else if (is_display_reset == true) // Show Display reset page.
 {
  web_page_str = web_page_header;
  web_page_str = web_page_str + "<body> " + HEADER4 + TITLE1;
  web_page_str = web_page_str + "<p>Display reset to factory defaults</p>";
  web_page_str = web_page_str + FORM1 + BUTTON1 + "formaction=\"/setup/home\" value=\"OK\"></form>";
  web_page_str = web_page_str + web_page_footer;

  err = send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
 }
 
 return err;
}

/*!
* \brief Processes user entered credentials.
*
* Three user entered fields: networkname, password and server URL.
* Check that each field (argument) is present, then extract them from the
* HTTP data section of the original request.
* Set the globals ssid, pass and server, writing them to the file system.
*
* If any argument is absent, or incorrect, display warning page with 
* relevant message.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param req HTTP request from client.
* \param rlen Length of HTTP request.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_image_server_credentials_page(struct tcp_pcb *pcb, char *req, int rlen)
{
 char *data;
 int len;

 bool is_data_changed = false;
 bool is_ssid_error = false;
 bool is_password_error = false;
 bool is_server_error = false;

 if ((!pcb) || (!req) || (rlen > MAX_CONTENTS_LENGTH))
  return ERR_ARG;

 data = strstr(req, "\r\n\r\n") + 4;  // HTTP data starts after "\r\n\r\n" sequence.
 len = rlen - (data - req);           // Length of arguments string.

// Check that ALL arguments are present.

 if ((strstr(data, NETWORK_NAME_ARGUMENT) == NULL) ||
     (strstr(data, PASSWORD_ARGUMENT) == NULL)     ||
     (strstr(data, SERVER_URL_ARGUMENT) == NULL))
 {
  memset(req, 0, rlen);  // Clear request buffer.
  return handle_error_message_page(pcb, REQUEST_ERROR, "/setup/imageserver");
 }

// Allow for zero length ssid and password. They may have been reset.

 new_ssid   = extract_argument(data, len, NETWORK_NAME_ARGUMENT);
 new_pass   = extract_argument(data, len, PASSWORD_ARGUMENT);
 new_server = extract_argument(data, len, SERVER_URL_ARGUMENT);

 memset(req, 0, rlen);  // Clear request buffer.

 log_text = "\n SSID:       " + new_ssid + "\n Password:   " + new_pass + "s\n Server URL: " + new_server + "\n";
 log->print_message(log_text);

// printf(" SSID:       %s\n Password:   %s\n Server URL: %s\n",
//        new_ssid.c_str(), new_pass.c_str(), new_server.c_str());  // *** Debug ***  ??

 is_ssid_present_and_correct = check_wifi_ssid_format();

 if ((is_ssid_present_and_correct == true) || (new_ssid.length() == 0))
 {
  if (new_ssid != ssid)
  {
   sh->set_wifi_ssid(new_ssid);
   ssid = new_ssid;
   is_data_changed = true;
  }
 }
 else
 {
  is_ssid_error = true;
 }

 is_password_present_and_correct = check_wifi_password_format();

 if ((is_password_present_and_correct == true) || (new_pass.length() == 0))
 {
  if (pass != new_pass)
  {
   sh->set_wifi_password(new_pass);
   pass = new_pass;
   is_data_changed = true;
  }
 }
 else
 {
  is_password_error = true;
 }

 is_server_url_present_and_correct = check_image_server_url_format();
 
 if ((is_server_url_present_and_correct == true) || (new_server.length() == 0))
 {
  if (server != new_server)
  {
   sh->set_image_server_url(new_server);
   server = new_server;
   is_data_changed = true;
  }
 }
 else
 {
  is_server_error = true;
 }

 // Set EPD Status.

 if (check_fields() == true)
 {
  if (sh->get_epd_status() != EPD_STORE_CREDENTIALS_SET)
  {
   sh->set_epd_status(EPD_STORE_CREDENTIALS_SET);
   is_data_changed = true;
  }
 }
 else
 {
  if (sh->get_epd_status() != EPD_STORE_SETTING_CREDENTIALS)
  {
   sh->set_epd_status(EPD_STORE_SETTING_CREDENTIALS);
   is_data_changed = true;
  }
 }

 // User entered data has changed - write it to flash.

 if ((is_data_changed == true) && 
     (is_ssid_error == false) && 
     (is_password_error == false) && 
     (is_server_error == false))
 {
  log->print_message("Writing credentials to data store.\n");  // *** Debug ***

  sh->write_data_to_store();
 }
 else if (is_ssid_error == true)
 {
  return handle_error_message_page(pcb, SSID_ERROR, "/setup/imageserver");
 }
 else if (is_password_error == true)
 {
  return handle_error_message_page(pcb, PASS_ERROR, "/setup/imageserver");
 }
 else if (is_server_error == true)
 {
  return handle_error_message_page(pcb, URL_ERROR, "/setup/imageserver");
 }

 return handle_home_page(pcb);
}

/*!
* \brief Processes user request to reset (clear) credentials.
*
* Sets credentials to empty strings.
* Re-displays image server credentials page.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_reset_image_server_credentials_page(struct tcp_pcb *pcb)
{
 if (!pcb)
  return ERR_ARG;

 new_ssid   = "";
 new_pass   = "";
 new_server = "";

 return handle_image_server_page(pcb);
}

/*!
* \brief Processes user request to cancel credentials.
*
* Returns credentials to previous (saved) values.
* Returns to home page.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_cancel_image_server_credentials_page(struct tcp_pcb *pcb)
{
 if (!pcb)
  return ERR_ARG;

 new_ssid   = ssid;
 new_pass   = pass;
 new_server = server;

 return handle_home_page(pcb);
}

/*!
* \brief Sends enter display mode page to client.
*
* Displays button allowing user to exit display configuration mode.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_change_display_mode_page(struct tcp_pcb *pcb) 
{
 string web_page_str;

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER8 + TITLE1 + DISPLAY_MODE_TITLE;
 web_page_str = web_page_str + "<p>Press OK to enter display mode.</p>";
 web_page_str = web_page_str + FORM1 + BUTTON1 + "formaction=\"/setup/displaymode\" value=\"OK\">&nbsp;&nbsp";
 web_page_str = web_page_str + BUTTON1 + "formaction=\"/setup/home\" value=\"Cancel\"></form>";
 web_page_str = web_page_str + web_page_footer;

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Exits display configuration mode.
*
* Sends exiting configuration message to the client.
* Stops the web server (closes the TCP socket connection).
* Changes system state from configuring display to display.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_setup_display_mode_page(struct tcp_pcb *pcb) 
{
 err_t err;
 string web_page_str;

 if (!pcb)
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER7 + TITLE1;
 web_page_str = web_page_str + "<p>Exiting configuration...</p>";
 web_page_str = web_page_str + web_page_footer;

 err = send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());

 stop_webserver(pcb);
 is_configuring = false;

 return err;
}

/*!
* \brief Sends error page to client.
*
* Displays the error page and a button that navigates to the web directory (page).
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param error_message Message to be displayed on the client.
* \param web_directory Web page to navigate to.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_error_message_page(struct tcp_pcb *pcb, string error_message, string web_directory)
{
 string web_page_str;

 if ((!pcb) || (error_message.empty()) || (web_directory.empty()))
  return ERR_ARG;

 web_page_str = web_page_header;
 web_page_str = web_page_str + "<body> " + HEADER6 + TITLE1 + ERROR_TITLE;
 web_page_str = web_page_str + "<p>" + error_message + "</p>";
 web_page_str = web_page_str + FORM1 + BUTTON1 + "formaction=\"" + web_directory + "\" value=\"OK\">";
 web_page_str = web_page_str + web_page_footer;

 return send_page(pcb, (char*)web_page_str.c_str(), web_page_str.length());
}

/*!
* \brief Handles client POST request.
*
* Extracts the path from the request and uses it to call the relevant
* page handler.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param req HTTP request from client.
* \param rlen Length of HTTP request.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_http_post(struct tcp_pcb *pcb, char *req, int rlen)
{
 err_t err = ERR_OK;
 char path[MAX_URL_LENGTH];

 if ((!pcb) || (!req) || (rlen > MAX_CONTENTS_LENGTH))
  return ERR_ARG;

 extract_path(path, req, rlen, HTTP_POST_OFFSET);

 if (strcmp(path, "setup/imageserver") == 0)
 {
  err = handle_image_server_page(pcb);
 }
 else if (strcmp(path, "setup/deviceid") == 0)
 {
  err = handle_device_id_page(pcb);
 }
 else if (strcmp(path, "setup/masterreset") == 0)
 {
  err = handle_master_reset_page(pcb);
 }
 else if (strcmp(path, "setup/resetconfirmed") == 0)
 {
  err = handle_reset_confirmed_page(pcb);
 }
 else if (strcmp(path, "setup/imageservercredentials") == 0)
 {
  err = handle_image_server_credentials_page(pcb, req, rlen);
 }
 else if (strcmp(path, "setup/resetimageservercredentials") == 0)
 {
  err = handle_reset_image_server_credentials_page(pcb);
 }
 else if (strcmp(path, "setup/cancelimageservercredentials") == 0)
 {
  err = handle_cancel_image_server_credentials_page(pcb);
 }
 else if (strcmp(path, "setup/display") == 0)
 {
  err = handle_change_display_mode_page(pcb);
 }
 else if (strcmp(path, "setup/displaymode") == 0)
 {
  err = handle_setup_display_mode_page(pcb);
 }
 else if (strcmp(path, "setup/home") == 0)
 {
  err = handle_home_page(pcb);
 }
 else
 {
  err = handle_page_not_found(pcb);
 }

 return err;
}

/*!
* \brief Handles client GET request.
*
* Extracts the path from the request and uses it to call the relevant
* page handler.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param req HTTP request from client.
* \param rlen Length of HTTP request.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::handle_http_get(struct tcp_pcb *pcb, const char *req, int rlen)
{
 err_t err = 0;
 char path[MAX_URL_LENGTH];

 if ((!pcb) || (!req) || (rlen > MAX_CONTENTS_LENGTH))
  return ERR_ARG;

 extract_path(path, req, rlen, HTTP_GET_OFFSET);

// Use path to determine next step(s).

 if (strcmp(path, "setup/home") == 0)
 {
  err = handle_home_page(pcb);
 }
 else
 {
  err = handle_page_not_found(pcb);
 }

 return err;
}

/*!
* \brief Decodes HTTP request.
*
* Searches for "GET" or "POST" in the request string and returns
* the request type.
*
* \param req HTTP request from client.
* \return http_req_type. [HTTP_GET | HTTP_POST | HTTP_UNKNOWN].
*/

enum http_req_type Credentials_Webserver::decode_http_request(const char *req)
{
 char const *get_str  = "GET";
 char const *post_str = "POST";

 if (!strncmp(req, get_str, strlen(get_str)))
  return HTTP_GET;

 if (!strncmp(req, post_str, strlen(post_str)))
  return HTTP_POST;

 return HTTP_UNKNOWN;
}

/*!
* \brief Calls the request handler based on the decoded request type.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param http_req HTTP request from client.
* \param http_req_len Length of HTTP request.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::generate_response(struct tcp_pcb *pcb, char *http_req, int http_req_len)
{
 if ((!pcb) || (!http_req) || (http_req_len > MAX_CONTENTS_LENGTH))
  return ERR_ARG;

 enum http_req_type request_type = decode_http_request(http_req);

 switch (request_type) 
 {
  case HTTP_GET:
       return handle_http_get(pcb, http_req, http_req_len);
  case HTTP_POST:
       return handle_http_post(pcb, http_req, http_req_len);
  default:
       printf("request_type != GET|POST\r\n");
       return -1;
 }
}

/*!
* \brief Stops the webserver.
*
* Sets the send and receive call back function pointers to NULL and 
* closes the TCP socket.
*
* \param pcb Pointer to the TCP protocol control block of the socket.
*/

void Credentials_Webserver::stop_webserver(struct tcp_pcb *pcb) 
{
 tcp_recv(pcb, NULL);
 tcp_sent(pcb, NULL);
 tcp_close(pcb); 
}

/*!
* \brief Sent callback - does nothing.
*
* Called from C wrapper function w_http_sent_callback().
*
* \param arg Not used.
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param len Length of HTTP request.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::http_sent_callback(void *arg, struct tcp_pcb *pcb, u16_t len)
{
 return ERR_OK;
}

/*!
* \brief Receive callback.
*
* If the pcb state is ESTABLISHED, acknowledge the receipt of the payload
* and generate a response to the client's request.
* Called from C wrapper function w_http_recv_callback().
*
* \param arg Not used.
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param p   Packet.
* \param err Error code.
* \return err_t. If < 0, an error occurred.
*/

err_t Credentials_Webserver::http_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
 err_t l_err = ERR_OK;

 if ((err != ERR_OK) || (!pcb) || (!p)) 
 {
  stop_webserver(pcb);
  return ERR_ARG;
 }

// Do not read the packet if we are not in ESTABLISHED state.

 if (pcb->state >= FIN_WAIT_1) 
 {
  pbuf_free(p);
  return -1;
 }

// Acknowledge that we've read the payload.

 tcp_recved(pcb, p->len);

// Read and decipher the request.

// This function takes care of generating a request, sending it,
// and closing the connection if all data can been sent. If
// not, then it sets up the appropriate arguments to the sent
// callback handler.
 
 l_err = generate_response(pcb, (char*)p->payload, p->len);
 pbuf_free(p);  // Free received packet.

 return l_err;
}

/*!
* \brief Starts web server.
*
* Creates a TCP protocol control block. 
* Binds the PCB to the wildcard IPV4 IP address and port 80.
* Starts listening for connections and specifies the callback 
* function (http_accept_callback) for incoming connections from the client.
*
* Note: 1. The PCB is re-allocated to reduce memory usage.
*       2. No arguments are needed as no files are to be requested.
*
*/

void Credentials_Webserver::start_webserver()
{
 struct tcp_pcb *pcb;
 err_t err;

 pcb = tcp_new();  // Create new TCP PCB structure.

 if (!pcb) 
 {
  log->print_message("Error creating PCB. Out of Memory\r\n");
  return;
 }

 err = tcp_bind(pcb, IP_ADDR_ANY, HTTP_PORT);  // Bind to http port 80.

 if (err != ERR_OK) 
 {
  log->print_message("Unable to bind to port 80\r\n");
// printf("Unable to bind to port 80: err = %d\r\n", err);
  return;
 }

 tcp_arg(pcb, NULL);     // No arguments required.
 pcb = tcp_listen(pcb);  // Listen for connections.

 if (!pcb) 
 {
  log->print_message("Out of memory while tcp_listen\r\n");
  return;
 }

 tcp_accept(pcb, http_accept_callback);  // Specify callback to use for incoming connections.
}

/*!
* \brief Extract URL's path from HTTP GET or POST request.
*
* \param path Pointer to path.
* \param req  HTTP request.
* \param rlen Length of HTTP request.
* \param initial_offset  Length of "GET" or "POST".
*/

void Credentials_Webserver::extract_path(char *path, const char *req, int rlen, int initial_offset)
{
 char *fstart, *fend;
 int offset = initial_offset;

// Locate the start of the path in the request.
// Requests are of the form GET /path/to/filename HTTP...

 if (req[offset] == '/')
  offset++;

 fstart = (char*)(req + offset);   // Start marker.

// Path finally ends in a space.

 while (req[offset] != ' ')
  offset++;

 fend = (char*)(req + offset - 1); // End marker.

 if (fend < fstart) // No path found, default to home page.
 {
  strcpy(path, "setup/home");
  return;
 }

// Malformed URL, or URL too long: set path to "NOT_FOUND".
// This causes the "Page not found" page to be displayed.

 if (offset > rlen || fend - fstart > MAX_URL_LENGTH) 
 {
  *fend = 0;
  strcpy(path, "NOT_FOUND");
  return;
 }

 strncpy(path, fstart, (fend - fstart + 1));  // Copy over the path...
 path[fend - fstart + 1] = 0;                 // ...and terminate it.
}

/*!
* \brief Extract argument value from data string.
*
* \param data  String containing arguments and their values.
* \param len   Length of data string.
* \param argument_name Argument's name whose value is to be extracted.
* \return string containing argument's value.
*/

string Credentials_Webserver::extract_argument(const char* data, int len, const char* argument_name)
{
 string argument_value = "";
 char* start_ptr;  // Points to start of argument's value.
 char* end_ptr;    // Points to end of argument's value.
 
 // Set start pointer.
 // Look for argument's name, then add length of argument's name
 // plus 1 for the '='.

 start_ptr = strstr(data, argument_name);

 if (start_ptr != NULL)
 {
  start_ptr += (strlen(argument_name) + 1);  // Take account of the '=' character.
 }

 // Set end pointer.
 // Look for the argument separator "&".
 // If not found, set it to the end of the data.

 end_ptr = strstr(start_ptr, "&");

 if (end_ptr == NULL)
  end_ptr = (char*)data + len - 1;
 else
  end_ptr -= 1;

 argument_value.assign(start_ptr, (end_ptr - start_ptr + 1));

 return argument_value;
}

// *** End of class definition ***

// *** Start of C functions ***

/*!
* \brief Wrapper function for tcp_recv().
*
* Allows C function to call C++ method http_recv_callback().
*
* \param arg Not used.
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param p   Packet.
* \param err Error code.
*/

err_t w_http_recv_callback(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
 return cws->http_recv_callback(arg, pcb, p, err);
}

/*!
* \brief Wrapper function for tcp_sent().
*
* Allows C function to call C++ method http_sent_callback().
*
* \param arg Not used.
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param len Length of HTTP request.
*/

err_t w_http_sent_callback(void *arg, struct tcp_pcb *pcb, u16_t len)
{
 return cws->http_sent_callback(arg, pcb, len);
}

/*!
* \brief Callback function used by tcp_accept().
*
* C function that assigns wrapper callback
* functions to tcp_rev() and tcp_sent().
*
* \param arg Not used.
* \param pcb Pointer to the TCP protocol control block of the socket.
* \param err Error code.
*/

err_t http_accept_callback(void *arg, struct tcp_pcb *pcb, err_t err)
{
 tcp_arg(pcb, NULL);

 tcp_recv(pcb, w_http_recv_callback);
 tcp_sent(pcb, w_http_sent_callback);

 return ERR_OK;
}

// *** End of C functions ***


