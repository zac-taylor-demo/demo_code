# credentials_webserver

The credentials webserver allows a user to configure a device containing a Pico-W using a web browser.
Upon initialisation, the Pico-W acts as a wireless access point to which a web client can connect.

Written in C/C++, the embedded application is designed to be compact, dynamically creating the web pages according to the web client requests.

An HTTP message handler was built on top of the existing lwip library, using a TCP socket that accepts connections on port 80. No encoding or file upload/download is used.

Note: the files dhcpserver.h and dhcpserver.c  were written by Damien P. George.

In order to build this, you will need the Pico SDK and CMake (the IDE was Visual Studio).
